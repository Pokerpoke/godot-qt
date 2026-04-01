#pragma once

#include <string>
#include <vector>
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
    LibGodot(std::string program_name, std::string project_path)
    {
        std::vector<std::string> arg_strings = {program_name, "--path", project_path,
                                                "--window-visible", "false"};

        // Convert to char* array
        std::vector<char *> args;
        for (auto &arg : arg_strings)
        {
            args.push_back(const_cast<char *>(arg.c_str()));
        }
        m_extension_obj =
            libgodot_create_godot_instance((int)args.size(), args.data(), init_callback);
        godot::Object *obj = godot::internal::get_object_instance_binding(m_extension_obj);
        m_godot_instance   = static_cast<godot::GodotInstance *>(obj);
        m_godot_instance->start();
        if (!m_godot_instance)
        {
            spdlog::error("Failed to get GodotInstance from GDExtensionObjectPtr");
        }

        m_main_loop = godot::Engine::get_singleton()->get_main_loop();
    }

    ~LibGodot()
    {
        libgodot_destroy_godot_instance(m_extension_obj);
    }

    void start()
    {
    }

    godot::Window *get_root()
    {
        godot::SceneTree *tree = godot::Object::cast_to<godot::SceneTree>(m_main_loop);
        return tree->get_root();
    }

    godot::Node *current_scene()
    {
        godot::SceneTree *tree = godot::Object::cast_to<godot::SceneTree>(m_main_loop);
        return tree->get_current_scene();
    }

public:
    godot::MainLoop *m_main_loop           = nullptr;
    GDExtensionObjectPtr m_extension_obj   = nullptr;
    godot::GodotInstance *m_godot_instance = nullptr;
};