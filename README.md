# Bitform Font

An experiment to see which AI model can create the best C CLI. I'm using it to produce a little app so I can draw ascii headings in my text editor using my Bitform font. The app itself is just AI gunk for a bit of fun. You can find the prompts in the `prompts` directory.

## Design

The important bit is the font itself, which I designed myself, and which you are free to use for anything. I needed a minimal 2-character tall Ascii font to use with my text editor, using block characters for readability. I was previously using [this ascii text generator](https://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20), but all the block character fonts I found were closer to 4 or 5 characters tall, which is a bit excessive. (Most of these are also incomplete, and don't contain all ascii glyphs.)

![Bitform font glyphs](https://github.com/UliTroyo/bitform-font/blob/main/Bitform.svg)

## Glyphs

To view, these require braille sextant glyphs form the [Symbols for Legacy Computing Unicode block](https://en.wikipedia.org/wiki/Symbols_for_Legacy_Computing#), which were the block mosaic set used by [the BBC Micro](https://en.wikipedia.org/wiki/Box-drawing_characters#BBC_and_Acorn).

```
|                                                             |
|  🬜🬊🬓 🬕🬂🬓 🬔🬂🬃 🬕🬈🬏 🬕🬂🬀 🬕🬂🬀 🬜🬂🬀 ▌ ▌ 🬂🬕🬀 🬁🬨🬀 ▌🬖🬀 ▌   🬛🬏🬖▌       |
|  ▌🬂▌ 🬲🬰🬃 🬣🬭🬃 🬲🬵🬄 🬴🬰🬏 🬕🬂  🬣🬯🬄 🬕🬂▌ 🬭🬲🬏 🬢🬘  🬕🬋🬏 🬲🬭🬏 ▌🬁 ▌       |
|                                                             |
|                                                             |
|  🬛🬏▌ 🬔🬂🬓 🬕🬂🬓 🬔🬂🬓 🬕🬂🬓 🬳🬂🬃 🬂🬕🬀 ▌ ▌ ▌ ▌ 🬣🬞🬞🬄 🬣🬞🬄 🬣🬞🬄 🬂🬡🬄       |
|  ▌🬈▌ 🬣🬭🬄 ▌🬂  🬣🬗🬐 ▌🬂🬓 🬢🬰🬃  ▌  🬣🬭🬄 🬉🬘  🬁🬔🬔  🬖🬈🬏  ▌  🬵🬮🬏       |
|                                                             |
|                                                             |
|  🬖▌ 🬅🬂🬓 🬁🬰🬄 🬦🬧  🬴🬰🬀 🬖🬡🬀 🬂🬡🬄 🬤🬰🬃 🬔🬂🬓 🬜🬊🬓                     |
|   ▌ 🬳🬰🬏 🬢🬭🬄 🬌🬫🬃 🬢🬭🬄 🬪🬮🬄 🬦🬀  🬣🬭🬄 🬠🬰🬄 🬪🬵🬄                     |
|                                                             |
|            🬞                🬭  🬞🬏                           |
|  ▌ 🬄🬄 🬵🬵🬏 🬤🬸🬰  🬃🬞🬃 🬤🬡🬃  🬞🬄 🬔    🬁🬓 🬢🬣🬃  🬓           🬞🬄 🬃    |
|  🬐    🬫🬫🬃 🬢🬷🬭🬄 🬖🬀🬏 🬣🬗🬘🬀    🬣    🬞🬄 🬀🬄🬀 🬂🬕🬀  🬓 🬂🬂🬀 🬏 🬔  🬃    |
|            🬁                🬂  🬁🬀          🬁                |
|      🬞      🬞            🬭     🬞🬏             🬭    🬞🬏       |
|   🬏 🬖🬀  🬋🬋🬃  🬈🬏 🬅🬡🬃 🬜🬡🬒🬓 ▌  🬣   ▌ 🬖🬈🬏     🬁🬃 ▐   ▌  ▐  🬞🬏🬏  |
|   🬓 🬁🬢  🬋🬋🬃 🬞🬅   🬐  ▌🬣🬣🬄 ▌  🬁🬓  ▌     🬭🬭🬏    🬧   ▌  🬦🬀 🬀🬂   |
|  🬁                       🬂     🬁🬀             🬂    🬁🬀       |
```

## TODO

- Add a web interface!
- ~~Add an SVG image so that the font is viewable even without font support for the Unicode block.~~
