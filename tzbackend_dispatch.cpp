#include "tzbackend_dispatch.h"

namespace tz
{
namespace renderer
{

#define TZ_BACKEND_DISPATCH_DEF(name) Command::DispatchFunction name::dispatch = NULL

TZ_BACKEND_DISPATCH_DEF(UploadNUniforms);
TZ_BACKEND_DISPATCH_DEF(DrawIndexed);

void (*clear_backbuffer)(GraphicsBitfield) = NULL;

}
}
