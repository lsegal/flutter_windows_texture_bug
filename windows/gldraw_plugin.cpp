#include "include/gldraw/g_l_draw_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace
{
  class GLDrawPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    GLDrawPlugin(flutter::TextureRegistrar *textures);

    virtual ~GLDrawPlugin();

  private:
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    std::unique_ptr<flutter::TextureVariant> tex_;
    FlutterDesktopPixelBuffer buf_;
    flutter::TextureRegistrar *texRegistrar_;
  };

  // static
  void GLDrawPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "gldraw",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<GLDrawPlugin>(registrar->texture_registrar());

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  GLDrawPlugin::GLDrawPlugin(flutter::TextureRegistrar *textures)
      : texRegistrar_(textures)
  {
    buf_.buffer = nullptr;
  }

  GLDrawPlugin::~GLDrawPlugin() {}

  void GLDrawPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    auto args = method_call.arguments();
    if (method_call.method_name().compare("open") == 0)
    {
      // callback to be called on each "MarkTextureFrameAvailable" call
      auto cb = flutter::PixelBufferTexture([=](size_t width, size_t height) {
        std::cout << "Expecting to fill " << width << " x " << height << " frame" << std::endl;

        // release last frame
        if (buf_.buffer != nullptr)
        {
          delete buf_.buffer;
        }

        // create a new frame so we always have clean data
        buf_.width = width;
        buf_.height = height;
        buf_.buffer = new uint8_t[width * height * 4];
        for (int i = 0; i < buf_.width * buf_.height * 4; i++)
        {
          ((uint8_t *)buf_.buffer)[i] = (uint8_t)rand();
        }

        // return a pointer, this will still be valid since buf is stored
        // "globally" on the plugin itself.
        return &buf_;
      });

      // register the texture with the callback
      tex_ = std::make_unique<flutter::TextureVariant>(cb);
      auto tex_id = texRegistrar_->RegisterTexture(tex_.get());
      result->Success(flutter::EncodableValue(flutter::EncodableValue(tex_id)));
    }
    else if (method_call.method_name().compare("draw") == 0)
    {
      // just trigger a draw from registrar on texture
      texRegistrar_->MarkTextureFrameAvailable(std::get<int64_t>(*args));
      result->Success();
    }
    else if (method_call.method_name().compare("dispose") == 0)
    {
      if (buf_.buffer != nullptr)
      {
        delete buf_.buffer;
      }
      tex_.release();
      result->Success();
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace

void GLDrawPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  GLDrawPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
