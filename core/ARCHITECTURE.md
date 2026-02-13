# Arquitetura de Renderização Multi-API

## Visão Geral

Este projeto implementa uma arquitetura de renderização flexível que suporta múltiplas APIs gráficas:
- **OpenGL 3.3+** (desktop nativo)
- **WebGL 2.0** (navegador via Emscripten)
- **Vulkan 1.2+** (desktop nativo)

## Estrutura de Abstração

### 1. Backend de Renderização (`RendererBackend`)

Interface base que define operações comuns:
```cpp
class RendererBackend {
    virtual bool init() = 0;
    virtual void clear() = 0;
    virtual void draw(const Mesh&) = 0;
    virtual void setUniforms(unsigned int) = 0;
    // ...
};
```

**Implementações:**
- `OpenGLRendererBackend` - OpenGL 3.3 Core Profile
- `WebGLRendererBackend` - WebGL 2.0 (ES 3.0)
- `VulkanRendererBackend` - Vulkan 1.2

### 2. Sistema de Shaders

**Abstração:**
- `ShaderCompiler` - Compila shaders para a API específica
- `ShaderProgram` - Gerencia programas/pipelines de shader
- `ShaderAsset` - Representa um shader compilado

**Pipeline de Compilação:**
```
HLSL (.vxs/.pxs) → SPIR-V (.spv) → GLSL (.glsl) [OpenGL/WebGL]
                                 → SPIR-V direto [Vulkan]
```

**Factories:**
- `ShaderCompilerFactory::create(GraphicsAPI, context)`
- `ShaderProgramFactory::create(GraphicsAPI, context)`

### 3. Sistema de Geometria

**Abstração:**
- `MeshBuffer` - Gerencia buffers de vértices/normais
- `Mesh` - Representa geometria renderizável

**Implementações:**
- `OpenGLMeshBuffer` - VAO/VBO
- `WebGLMeshBuffer` - WebGL buffers
- `VulkanMeshBuffer` - VkBuffer + VkDeviceMemory

**Factory:**
- `MeshBufferFactory::create(GraphicsAPI, context)`

### 4. Gerenciamento de Janelas

`WindowManager` cria janelas apropriadas para cada API:
- OpenGL/WebGL: `SDL_WINDOW_OPENGL` + `SDL_GL_CreateContext`
- Vulkan: `SDL_WINDOW_VULKAN` + `SDL_Vulkan_CreateSurface`

## Fluxo de Inicialização

### OpenGL/WebGL
```
1. WindowManager cria backend (OpenGL/WebGL)
2. Backend::initWindowContext() configura atributos SDL
3. WindowManager cria janela SDL_WINDOW_OPENGL
4. WindowManager cria contexto OpenGL
5. Backend::init() inicializa GLEW, UBOs, etc
```

### Vulkan
```
1. WindowManager cria VulkanRendererBackend
2. Backend::initWindowContext() (no-op para Vulkan)
3. Backend cria VkInstance
4. WindowManager cria janela SDL_WINDOW_VULKAN
5. WindowManager cria VkSurfaceKHR via SDL
6. Backend::init() cria device, swapchain, render pass, etc
```

## Diferenças Chave entre APIs

### OpenGL
- Estado global (bind/unbind)
- Shaders compilados em runtime de GLSL
- UBOs para uniforms compartilhados
- VAO/VBO para geometria

### Vulkan
- Explícito e verboso
- SPIR-V carregado diretamente
- Descriptor sets para recursos
- Command buffers para gravação de comandos
- Sincronização manual (semaphores, fences)
- Swapchain manual

### WebGL
- Subset de OpenGL ES 3.0
- Sem GLEW (funções nativas)
- Restrições de segurança do navegador
- Emscripten para compilação

## Próximos Passos para Vulkan

### Implementações Pendentes

1. **Swapchain Completo**
   - Query de capabilities
   - Seleção de formato/present mode
   - Criação de image views
   - Framebuffers

2. **Pipeline Gráfico**
   - Carregar shaders SPIR-V
   - Configurar vertex input
   - Viewport/scissor
   - Rasterização, depth test
   - Blend state

3. **Command Buffers**
   - Gravação de comandos de draw
   - Submit para queue
   - Apresentação

4. **Recursos**
   - Uniform buffers (matrizes, lighting)
   - Descriptor sets
   - Texturas e samplers
   - Cubemap para skybox

5. **Sincronização**
   - Semaphores para image acquisition/present
   - Fences para CPU-GPU sync
   - Pipeline barriers

6. **Integração com Sistema Existente**
   - `draw()` deve gravar command buffer
   - `setUniforms()` deve atualizar descriptor sets
   - `renderSkybox()` precisa de pipeline separado

## Estrutura de Arquivos

```
src/
├── graphics_api.hpp              # Enum GraphicsAPI
├── renderer_backend.hpp          # Interface base
├── renderer.hpp/cpp              # Renderer principal
│
├── open_gl_*                     # Implementação OpenGL
├── web_gl_*                      # Implementação WebGL
├── vulkan_*                      # Implementação Vulkan
│
├── shader_compiler.hpp           # Interface shader compiler
├── shader_program.hpp            # Interface shader program
├── mesh_buffer.hpp               # Interface mesh buffer
│
├── *_factory.hpp/cpp             # Factories para criar objetos por API
│
└── window_manager.hpp/cpp        # Gerenciamento de janelas
```

## Compilação

### Native (OpenGL)
```bash
cmake -B build
cmake --build build
```

### Native (Vulkan)
Altere em `main.cpp`:
```cpp
GraphicsAPI graphicsAPI = GraphicsAPI::VULKAN;
```

### WebGL (Emscripten)
```bash
emcmake cmake -B build
cmake --build build
```

## Dependências

- **SDL2** - Janelas e input
- **GLEW** - Carregamento de extensões OpenGL (nativo)
- **GLM** - Matemática
- **Vulkan SDK** - Headers e bibliotecas Vulkan
- **DXC** - Compilador HLSL → SPIR-V
- **SPIRV-Cross** - Transpiler SPIR-V → GLSL

## Vantagens da Arquitetura

1. **Separação de Concerns**: Lógica de jogo independente de API gráfica
2. **Testabilidade**: Backends podem ser mockados
3. **Manutenibilidade**: Mudanças em uma API não afetam outras
4. **Extensibilidade**: Fácil adicionar DirectX 11/12, Metal, etc
5. **Portabilidade**: Mesmo código roda em desktop e web
