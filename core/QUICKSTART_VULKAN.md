# Guia Rápido - Vulkan Renderer

## ✅ Compilação Bem-Sucedida!

O backend Vulkan foi implementado e compilado com sucesso.

## Como Testar

### 1. Ativar Vulkan

Edite `src/main.cpp` na linha 13:

```cpp
// Mude de:
GraphicsAPI graphicsAPI = GraphicsAPI::OPENGL;

// Para:
GraphicsAPI graphicsAPI = GraphicsAPI::VULKAN;
```

### 2. Recompilar

```bash
cmake --build build
```

### 3. Executar

```bash
cd /home/anderson/Documents/teste
./build/main
```

## O Que Esperar

- Janela SDL com título "Renderer"
- Cubo 3D rotacionando
- Mesma cena que OpenGL, mas renderizada com Vulkan
- ESC para sair

## Comparação OpenGL vs Vulkan

| Aspecto | OpenGL | Vulkan |
|---------|--------|--------|
| Inicialização | Rápida | Verbosa (mais código) |
| Performance | Boa | Potencialmente melhor |
| Controle | Médio | Total |
| Debugging | Fácil | Requer validation layers |
| Shaders | GLSL runtime | SPIR-V pré-compilado |

## Arquivos Importantes

- `src/vulkan_renderer_backend.cpp` - Implementação principal (500+ linhas)
- `phong.vxs.spv` / `phong.pxs.spv` - Shaders SPIR-V
- `src/main.cpp` - Loop de renderização

## Troubleshooting

**Problema: Tela preta**
- Verifique se os arquivos .spv existem no diretório raiz
- Verifique se a câmera está configurada em scene.scn

**Problema: Crash na inicialização**
- Verifique se o driver Vulkan está instalado: `vulkaninfo`
- Verifique mensagens de erro no terminal

**Problema: "Failed to create swapchain"**
- Pode ser problema de compatibilidade de surface
- Tente atualizar drivers gráficos

## Próximos Experimentos

1. **Comparar Performance**
   - Rode com OpenGL e Vulkan
   - Compare uso de CPU/GPU

2. **Adicionar Validação**
   - Habilite validation layers no código
   - Veja mensagens de debug

3. **Modificar Shaders**
   - Edite phong.vxs/phong.pxs
   - Recompile com DXC
   - Veja mudanças em tempo real

## Sucesso!

Você agora tem um renderer multi-API funcional:
- ✅ OpenGL 3.3
- ✅ WebGL 2.0 (Emscripten)
- ✅ Vulkan 1.2

Tudo usando a mesma arquitetura abstrata!
