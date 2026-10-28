#include <iostream>
#include <memory>

class Window {
public:
    virtual void draw() const = 0;
    virtual ~Window() = default;
};

class Button {
public:
    virtual void draw() const = 0;
    virtual ~Button() = default;
};

class Shader {
public:
    virtual void compile() const = 0;
    virtual ~Shader() = default;
};

class OpenGLWindow : public Window {
public:
    void draw() const override {
        std::cout << "[OpenGL] Ventana: crea contexto GL y limpia el framebuffer" << std::endl;
    }
};

class OpenGLButton : public Button {
public:
    void draw() const override {
        std::cout << "[OpenGL] Boton: dibuja un quad en modo inmediato" << std::endl;
    }
};

class OpenGLShader : public Shader {
public:
    void compile() const override {
        std::cout << "[OpenGL] Shader: compila GLSL en tiempo de ejecucion" << std::endl;
    }
};

class VulkanWindow : public Window {
public:
    void draw() const override {
        std::cout << "[Vulkan] Ventana: crea surface y swapchain" << std::endl;
    }
};

class VulkanButton : public Button {
public:
    void draw() const override {
        std::cout << "[Vulkan] Boton: graba comandos en un command buffer" << std::endl;
    }
};

class VulkanShader : public Shader {
public:
    void compile() const override {
        std::cout << "[Vulkan] Shader: carga bytecode SPIR-V precompilado" << std::endl;
    }
};

class RenderFactory {
public:
    virtual std::unique_ptr<Window> createWindow() const = 0;
    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<Shader> createShader() const = 0;
    virtual ~RenderFactory() = default;
};

class OpenGLFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() const override {
        return std::make_unique<OpenGLWindow>();
    }
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<OpenGLButton>();
    }
    std::unique_ptr<Shader> createShader() const override {
        return std::make_unique<OpenGLShader>();
    }
};

class VulkanFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() const override {
        return std::make_unique<VulkanWindow>();
    }
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<VulkanButton>();
    }
    std::unique_ptr<Shader> createShader() const override {
        return std::make_unique<VulkanShader>();
    }
};

int main() {
    std::unique_ptr<RenderFactory> factory =
        std::make_unique<OpenGLFactory>();
    auto window = factory->createWindow();
    auto button = factory->createButton();
    window->draw();
    button->draw();
    factory = std::make_unique<VulkanFactory>();
    auto shader = factory->createShader();
    shader->compile();
}
