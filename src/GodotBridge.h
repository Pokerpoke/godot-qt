#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/godot_instance.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/main_loop.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <vulkan/vulkan.h>

// actual API
extern "C"
{
    GDExtensionObjectPtr libgodot_create_godot_instance(
        int p_argc, char *p_argv[], GDExtensionInitializationFunction p_init_func);
    void libgodot_destroy_godot_instance(GDExtensionObjectPtr p_godot_instance);
}

// Empty callbacks for GDExtension initialization
static void initialize_callback(godot::ModuleInitializationLevel level)
{
    // Empty - no extension initialization needed
}

static void deinitialize_callback(godot::ModuleInitializationLevel level)
{
    // Empty - no extension cleanup needed
}

// GDExtension initialization function
static GDExtensionBool init_callback(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                     GDExtensionClassLibraryPtr p_library,
                                     GDExtensionInitialization *r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_callback);
    init_obj.register_terminator(deinitialize_callback);
    init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_CORE);
    return init_obj.init();
}

class LibGodot
{
public:
    LibGodot()
    {
    }

    LibGodot(std::string program_name, std::string project_path)
    {
        initialize({program_name, "--path", project_path});
    }

    static LibGodot *instance()
    {
        static LibGodot instance("godot", "../project");
        return &instance;
    }

    void initialize(const std::vector<std::string> &args_vec)
    {

        // Convert to char* array
        std::vector<char *> args;
        for (const auto &arg : args_vec)
        {
            args.push_back(const_cast<char *>(arg.c_str()));
        }
        m_extension_obj =
            libgodot_create_godot_instance((int)args.size(), args.data(), init_callback);
        godot::Object *obj = godot::internal::get_object_instance_binding(m_extension_obj);
        m_godot_instance   = static_cast<godot::GodotInstance *>(obj);
        m_godot_instance->start();

        m_main_loop = godot::Engine::get_singleton()->get_main_loop();
    }

    ~LibGodot()
    {
        libgodot_destroy_godot_instance(m_extension_obj);
    }

    godot::Window *get_root()
    {
        godot::SceneTree *tree = godot::Object::cast_to<godot::SceneTree>(m_main_loop);
        return tree->get_root();
    }

    godot::Node *get_node(const std::string &path)
    {
        return get_root()->get_node_or_null(path.c_str());
    }

    VkInstance get_vkinstance()
    {
        auto rs         = godot::RenderingServer::get_singleton();
        auto rd         = rs->get_rendering_device();
        auto vkinstance = rd->get_driver_resource(
            godot::RenderingDevice::DRIVER_RESOURCE_TOPMOST_OBJECT, godot::RID(), 0);
        return VkInstance(vkinstance);
    }

    godot::Node *current_scene()
    {
        godot::SceneTree *tree = godot::Object::cast_to<godot::SceneTree>(m_main_loop);
        return tree->get_current_scene();
    }

    bool iteration()
    {
        return m_godot_instance->iteration();
    }

    struct ImageData
    {
        int width  = 0;
        int height = 0;
        std::vector<uint8_t> data;
    };

private:
    godot::MainLoop *m_main_loop           = nullptr;
    GDExtensionObjectPtr m_extension_obj   = nullptr;
    godot::GodotInstance *m_godot_instance = nullptr;
};