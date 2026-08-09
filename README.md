# Coin-Op Stellar Evolution

A one-credit-one-star arcade stellar evolution simulator for Pac-Man-class Z80 hardware,
written for [Z88DK](https://z88dk.org/) with C gameplay code and a small Z80 assembly
hardware layer.

The game loop is intentionally coin-op simple:

1. Insert coin.
2. Generate one random protostar.
3. Spend a limited number of stellar interventions.
4. Watch the star evolve through accelerated phases.
5. Score rare outcomes.
6. Record notable stars on the high-score table.

## Build

Install Z88DK and make sure `zcc` is on your `PATH`, then run:

```sh
make
```

The default target builds a Pac-Man/Midway-style Z80 binary using the `+embedded`
target and places outputs in `build/`. If your local Pac-Man ROM toolchain expects a
specific split-ROM layout, use `build/stellar.bin` as the generated payload.

Useful targets:

```sh
make          # compile the simulator
make clean    # remove build artifacts
```

## Controls

The hardware input shim maps Pac-Man input bits into a compact control byte:

| Control | Action |
| --- | --- |
| Coin | Start a new credit/star |
| Start 1 | Confirm / advance |
| Left | Previous intervention |
| Right | Next intervention |
| Button | Apply selected intervention |

## Gameplay Model

Each credit generates a protostar with mass, metallicity, rotation, magnetic field,
and possible binary companion. The player receives three interventions, then the
simulation computes a fast, arcade-readable fate such as red dwarf, red giant,
Wolf-Rayet star, supernova, neutron star, magnetar, black hole, or pair-instability
supernova.
