# slush - The stupid and lightly underwhelming shell

This repository is home to `slush`, my take on implementing a shell in C using flex and bison.
The program originates from a task of the [complex lab systems programming](http://www.inf.tu-dresden.de/index.php?node_id=1336&ln=en) I undertook as part of my Master's studies at the TU Dresden.

## Requirements

The shell itself was built using:

- C11
- bison version 3.3.2
- flex version 2.6.4

The latter two can be installed on macOS using `brew install flex bison`.

## Building the Code

In order to compile the project, just use the makefile provided with this repo and run:

```shell
make generate # generates the scanner and parser
make release
```

_A note to macOS users:_ The code will not compile using the stock versions of flex and bison, which are both way older than the homebrew versions.

## License

This work is licensed under the MIT license.
