# Red Shift

![title screen](./assets/title_screen/title_screen.png)

This is a tiny, in-progress [Asteroids](https://en.wikipedia.org/wiki/Asteroids_(video_game)) clone for the [NES](https://en.wikipedia.org/wiki/Nintendo_Entertainment_System).
It's written using [neslib](https://github.com/clbr/neslib) and [famitone5](https://github.com/nesdoug/famitone5.0), and I utilized a bunch of different [tools](./tools) for formatting the graphics and audio in the correct way.
The build process makes heavy use of [cc65](https://cc65.github.io/).

The latest release can always be downloaded [here](https://github.com/CharlesAverill/RedShift/raw/refs/heads/main/RedShift.nes).
Eventually I'll host an in-browser version.

## Controls

| Control | Effect |
|---|---|
| Left/Right | Rotate ship counter/clockwise |
| Up | Thrust |
| Down | Face retrograde (sort of buggy) |
| A | Fire missile |

## Building

Install dependencies:
- The [cc65 toolchain](https://cc65.github.io/)
- [wine](https://www.winehq.org/)
- A suitable emulator (I prefer [mesen](https://www.mesen.ca/) for playing but [fceux](https://fceux.com/web/home.html) is also very good)

```bash
git clone https://github.com/CharlesAverill/RedShift.git
cd RedShift
make
```

## Artistic Credits

### Images

| Placement | Credit |
|---|---|
| Title screen black hole | [CNRS](https://www.cnrs.fr/en/press/first-ever-image-black-hole-cnrs-researcher-had-simulated-it-early-1979) |
| Background quasar | [NASA](https://science.nasa.gov/asset/webb/quasar-illustration/) |
| Background spiral galaxy | [NASA](https://science.nasa.gov/missions/hubble/hubble-captures-majestic-barred-spiral/) |
| Title screen font | [ITC Benguiat](https://en.wikipedia.org/wiki/ITC_Benguiat) |

### Music

| Placement | Credit |
|---|---|
| Background music | [Passou por mim e sorriu - Deolinda](https://www.youtube.com/watch?v=L_ymvjxErs0) |
| Game Over music | [Prelude, Op. 28, No. 4 - Chopin](https://en.wikipedia.org/wiki/Prelude,_Op._28,_No._4_(Chopin)) |
