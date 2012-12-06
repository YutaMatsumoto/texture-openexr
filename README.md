* OpenEXR compilation
  * openexr depends on ilmbase. Both of them can be found under [OpenEXR](http://www.openexr.com/downloads.html) 
  * How to compile openexr on linux : [ImageMagick • View topic - OpenExr support on Linux](http://www.imagemagick.org/discourse-server/viewtopic.php?f=2&t=16875)
  * Also, needed to modify some .cpp files which use memcpy() to have `#include <string.h>
