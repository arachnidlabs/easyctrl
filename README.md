easyctrl
========

Makes controlling and monitoring an Arduino sketch from a computer trivial!

## API

### Variables

Using easyctrl, you define monitored variables like this:

    Monitored<int> foo("foo");

A monitored variable has changes to its value automatically sent to clients listening on the serial port, and by default, accepts changes via serial port commands. Apart from the variable name, you need to specify the name you want it to be known by externally.

Optionally, you can also specify an initial value, and whether or not you want it to be writeable:

    Monitored<int> foo("foo", 42, NULL);

Specifying NULL for the third parameter signifies that the variable should not be writeable.

Once you've declared a monitored variable, you can use it just like you would a regular variable:

    foo = analogRead(A1); // Update foo
    digitalWrite(13, foo > 42); // Read foo

Currently, the following numeric types are supported: `int`, `unsigned int`, `long`, `unsigned long`, and `float`.

Basic support for fixed-size buffers is provided in the form of `MonitoredBuffer`. You define one like this:

    MonitoredBuffer<80> buf("buf");

This declares an 80 character long buffer. No dynamic memory allocation is required or used. Just like primitives, you can perform most ordinary operations on the buffer as if it were a char[]:

    buf[0] = 'X';
    char foo = buf[1];
    strcpy(buf, "Blah");

Unlike primitive variables, MonitoredBuffer can't always tell when you've modified its contents. If you've modified it directly, such as in the examples above, you'll need to tell it:

    buf.changed();

MonitoredBuffer also supports a few convenience methods for copying into it:

    buf = "Foo"; // Makes a copy of foo into buf
    buf = another_string; // As above, copies the value in another_string
    buf = F("I'm in progmem!"); // Copies a value out of flash into the buffer

### Change events

Sometimes it's useful to be able to respond when the remote end changes a variable. You can do this by declaring a change event handler. First, define a function that takes a pointer to the monitored variable as its only argument:

    void on_foo_change(Monitored<int> *foo) {
      bar = *foo * 2;
    }

Then, declare your monitored variable with the event handler as the third argument:

    Monitored<int> foo("foo", 42, on_foo_change);

Your change handler function will be called whenever the remote end changes the value of the variable. Local changes will not trigger the handler.

### Initialization & Execution

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

String values are supported with minimal escaping. \r, \n and \ are all escaped on output and unescaped on input.

Easyctrl currently accepts only one command, `.set`, which sets a variable. Its use is straightforward:

    .set foo 123

No response is returned to a valid `.set` command.

When setting a string value, the remainder of the command up to the trailing newline is taken as the new value for the buffer.

## Examples

See the examples directory for some examples.
