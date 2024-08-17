#include "src/core/opengl.hpp"

#include "src/core/numeric-string.hpp"
#include "src/ui/screen.hpp"
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSet>

std::optional<OpenGlInfo> OpenGlInfo::getForActiveDevice() {
  QScreen *screen = Screen::getActive();
  if (!screen)
    return std::optional<OpenGlInfo>();

  QSurfaceFormat preferredFormat;
  preferredFormat.setVersion(4, 6);

  QOpenGLContext context;
  context.setScreen(screen);
  context.setFormat(preferredFormat);
  if (!context.create())
    return std::optional<OpenGlInfo>();

  QOffscreenSurface surface(screen);
  surface.setFormat(context.format());
  surface.create();
  if (!surface.isValid())
    return std::optional<OpenGlInfo>();

  QOpenGLContext *prevContext = QOpenGLContext::currentContext();
  QSurface *prevSurface = prevContext ? prevContext->surface() : nullptr;

  if (!context.makeCurrent(&surface))
    return std::optional<OpenGlInfo>();

  string gpuName;
  string gpuVendor;
  string shaderVersion;

  QOpenGLFunctions *api = context.functions();
  if (api != nullptr) {
    const GLubyte *value;

    value = api->glGetString(GL_RENDERER);
    if (value)
      gpuName = (const char *)value;

    value = api->glGetString(GL_VENDOR);
    if (value)
      gpuVendor = (const char *)value;

    value = api->glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (value)
      shaderVersion = (const char *)value;
  }

  const QSet<QByteArray> qExtensions = context.extensions();
  HashSet<string> extensions;
  extensions.reserve(qExtensions.count());

  for (const QByteArray &ext : qExtensions) {
    extensions.insert(ext.data());
  }

  OpenGlInfo info(std::move(gpuName), std::move(gpuVendor),
                  Version{(ushort)context.format().majorVersion(),
                          (ushort)context.format().minorVersion(), (ushort)0},
                  std::move(shaderVersion), std::move(extensions));

  context.doneCurrent();
  if (prevSurface)
    prevContext->makeCurrent(prevSurface);

  return std::optional<OpenGlInfo>(std::move(info));
}

void OpenGlInfo::serialize(
    JsonWriter &jw,
    const std::initializer_list<const char *> &relevantExtensions) const {
  jw.writeObjectStart();
  jw.writeProperty("gpuName", name);
  jw.writeProperty("gpuVendor", vendor);
  jw.writeProperty("glVersion", Number::toString(glVersion.major) + '.' +
                                    Number::toString(glVersion.minor));
  jw.writeProperty("shaderVersion", shaderVersion);
  jw.writePropertyName("relevantExtensions");
  jw.writeObjectStart();
  for (const char *ext : relevantExtensions) {
    jw.writeProperty(ext, extensions.count(ext) > 0);
  }
  jw.writeObjectEnd();
  jw.writeObjectEnd();
}
