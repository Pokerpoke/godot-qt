#pragma once

#include "GodotBridge.h"
#include <QQuickItem>
#include <QTimer>
#include <QSGNode>
#include <QImage>
#include <QSGTexture>
#include <QSGSimpleTextureNode>

class GodotVKItem : public QQuickItem
{
    Q_OBJECT
public:
    GodotVKItem(QObject *parent = nullptr)
    {
        setFlag(QQuickItem::ItemHasContents, true);
        connect(&m_timer, &QTimer::timeout, this, &GodotVKItem::updateGodot);
        m_timer.setInterval(16);
        m_timer.start();
        m_libGodot = LibGodot::instance();
    }

    ~GodotVKItem()
    {
        delete qvk_instance;
        qvk_instance = nullptr;
    }

protected:
    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData) override
    {
        QSGSimpleTextureNode *node = nullptr;
        if (oldNode == nullptr)
        {
            node    = new QSGSimpleTextureNode;
            oldNode = node;
        }
        else
        {
            node = static_cast<QSGSimpleTextureNode *>(oldNode);
        }

        if (window() && m_vk_image != VK_NULL_HANDLE)
        {
            auto texture = QNativeInterface::QSGVulkanTexture::fromNative(
                m_vk_image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, window(), QSize(800, 600));

            node->setTexture(texture);
            node->setRect(boundingRect());
            node->setFiltering(QSGTexture::Linear);
            node->markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyGeometry);
        }

        return node;
    }

private:
    void update_vk_image()
    {
        auto view_port = m_libGodot->get_root()->get_node<godot::Viewport>("Node3D/SubViewport");
        auto tex       = view_port->get_texture();
        auto rid       = tex->get_rid();
        auto rs        = godot::RenderingServer::get_singleton();
        auto native_handle = rs->texture_get_native_handle(rid);
        m_vk_image         = VkImage((uint64_t)native_handle);
    }

    void updateGodot()
    {
        m_libGodot->iteration();

        if (!qvk_instance)
        {
            qvk_instance = new QVulkanInstance;

            auto vk_instance = LibGodot::instance()->get_vkinstance();
            qvk_instance->setVkInstance(vk_instance);
            window()->setVulkanInstance(qvk_instance);
        }
        update_vk_image();

        update();
    }

private:
    LibGodot *m_libGodot = nullptr;

    QTimer m_timer;
    bool vulkan_init              = false;
    QVulkanInstance *qvk_instance = nullptr;
    VkImage m_vk_image            = VK_NULL_HANDLE;
    QImage m_qimage;
};