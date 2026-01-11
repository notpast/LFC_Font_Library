# LFC Font Library  


LFC is an ultra-compact, memory-efficient bitmap font library designed specifically for resource-constrained embedded systems. With font files as small as 3-4KB and support for multiple displays, it provides professional typography capabilities for systems with less than 64KB of memory.


## Key Features

  ### Ultra-Compact Memory Footprint

    - Tiny font files: ~4KB for code space, ~3KB for 24px ASCII fonts

    - Zero heap usage: Completely malloc-free design

    - Minimal overhead: < 100 bytes RAM usage per display context

    - Optimized for: Systems with 64KB or less total memory  


 ### Full Unicode Support  

    - Complete UTF-8 encoding/decoding

    - Direct UTF-32 character rendering

    - Multi-language text capabilities

    - Efficient Unicode code point lookup


### Advanced Display Management  

    - Multiple display support: Drive different screens simultaneously

    - Dynamic screen rotation: 0°, 90°, 180°, 270° orientations

    - Display-independent rendering: Same API for all display types

    - Flexible pixel callbacks: Custom display driver integration


###  Professional Text Rendering

    - Configurable character spacing and padding

    - Text inversion (black/white mode)

    - Bounding box visualization

    - Rectangle drawing (filled or outline)

    - Multi-display synchronized operations



