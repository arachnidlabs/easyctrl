easyctrl
========

Makes controlling and monitoring an Arduino sketch from a computer trivial!

## API

Using easyctrl, you define monitored variables like this:

    Monitored<int> foo("foo");

A monitored variable has changes to its value automatically sent to clients listening on the serial port, and by default, accepts changes via serial port commands. Apart from the variable name, you need to specify the name you want it to be known by externally.

Optionally, you can also specify an initial value, and whether or not you want it to be writeable:

    Monitored<int> foo("foo", 42, false);

Once you've declared a monitored variable, you can use it just like you would a regular variable:

    foo = analogRead(A1); // Update foo
    digitalWrite(13, foo > 42); // Read foo

To initialize easyctrl, call `Easyctrl.begin()` in your setup function, like this:

    void setup() {
      Serial.begin(115200);
      Easyctrl.begin("test", Serial);
    }

The two arguments required are the name to identify as, and the communications interface, which will usually be your primary serial port, like in the example above.

In order for easyctrl to be able to work, you need to call `Easyctrl.update()` regularly. A good place to do this is in your main loop:

    void loop() {
      while(1) {
        Easyctrl.update();
        // Other stuff
      }
    }

Easyctrl only sends updates and receives commands when `Easyctrl.update()` is called. 

## Protocol

The wire protocol is extremely simple. When `Easyctrl.begin()` is called, easyctrl will output a manifest, which looks like this:

    .fields test
    .field foo int rw
    .field bar int ro
    .endfields

The introductory `.fields` line starts the manifest and provides the name of the sketch. Each `.field` line describes one field, specifying name, datatype, and whether the field is read-write or read-only. The manifest is terminated by a `.endfields` line.

After that, any changes to variables will be output the next time `Easyctrl.update()` is run, like this:

    .value foo 42

Easyctrl currently accepts only one command, `.set`, which sets a variable. Its use is straightforward:

    .set foo 123

No response is returned to a valid `.set` command.

## Examples

See the examples directory for some examples.
