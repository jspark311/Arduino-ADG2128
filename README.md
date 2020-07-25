# ADG2128

A library for Analog Devices' 8x12 analog cross-point switch.

#### [Hackaday.io Page](https://hackaday.io/project/167228-adg2128-breakout)

#### [Order from Tindie](https://www.tindie.com/products/17864/)

------------------------

### Notes on driver features:

#### Optional pins

The reset pin is optional and must be tied high if not under software control.
But it must still be passed as an argument to the constructor. If you have left the pin
disconnected, pass 255 for that pin, and the class will behave in a manner to
accommodate it (at the cost of time and complexity in the driver).

#### Preserve-on-destroy

When the driver instance for the switch is destroyed, the default behavior is to
put the hardware into a known state (reset). This opens all routes. Use-cases
that want the hardware state to outlive the driver's life cycle are possible by
setting preserveOnDestroy(true) ahead of `init()`, like so...

    // Class initializes with the existing state of the hardware.
    adg2128.preserveOnDestroy(true);
    adg2128.init(&Wire);

Since `init()` will also call `reset()` by default, preserveOnDestroy must be
enabled prior to `init()` to achieve the desired result (both sides of the driver life cycle).

#### Preserving and restoring hardware states

The driver provides a means of cloning prior states to and from a buffer.
Possible reasons for doing this might be...

  * Higher bus efficiency during `init()`.
  * Major code savings, since you can skip configuring the hardware and the class with discrete function calls.

Saving the current state can be done like this (from the example sketch)...

    uint8_t buf[ADG2128_SERIALIZE_SIZE];
    uint8_t written = adg2128.serialize(buf, ADG2128_SERIALIZE_SIZE);
    if (ADG2128_SERIALIZE_SIZE == written) {
      // Everything worked. Do what you will with the buffer.
    }

Restoring the state from a buffer can be done either upon construction...

    uint8_t buf[17] = {
      0x01, 0x70, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00
    };
    ADG2128 adg2128(buf, 17);

...or after a successful `init()`...

    // Literally anywhere in the sketch...
    adg2128.unserialize(buf, 17);

If done by construction, the configuration will be written to the class immediately,
and to the hardware during `init()`.

Invocation of `unserialize()` will immediately result in I/O as the new
configuration is imparted, but the fields (if any) that are otherwise required for
construction will be ignored.

#### Throwing many switches at once

The simple use of the driver is to change one switch at a time. But the API allows
an extra parameter to defer action until many switches have been changed, thus allowing
many concurrent changes in hardware at the same time. This is done like so...

    // Both of these calls close their switches immediately. Row 7 spends more
    //   time connected to column 4 than it does to column 5.
    adg2128.setRoute(4, 7);
    adg2128.setRoute(5, 7, false);  // Default value for 3rd parameter is false.

    // In this case, row 8 spends equal time connected to columns 1, 2, and 3.
    adg2128.setRoute(1, 8, true); // Change saved to hardware, but action deferred.
    adg2128.setRoute(2, 8, true); // Change saved to hardware, but action deferred.
    adg2128.setRoute(3, 8);       // This call also closes switches 1:8 and 2:8.

------------------------

### Dependencies

This class relies on [CppPotpourri](https://github.com/jspark311/CppPotpourri) for
debugging output. This dependency can be eliminated by culling those functions.
