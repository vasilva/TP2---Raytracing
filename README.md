# Raytracer - Guia Rápido

Ray tracer com distributed ray tracing (soft shadows e depth of field).

## Compilar e Executar

```bash
make
./raytracer scene1.txt output.ppm 800 600
```

## Controles (Janela GLUT)

- `ESC` - Sair
- `R` - Alternar ray tracing / OpenGL
- `1` - Soft Shadows (sombras suaves)
- `2` - Depth of Field (profundidade de campo)

## Distributed Ray Tracing

### Soft Shadows (Tecla 1)
Sombras com bordas suaves usando 4 amostras por luz.

### Depth of Field (Tecla 2)
Objetos fora de foco desfocados. Parâmetros:
- Abertura: 0.5
- Distância focal: 150.0
- Amostras: 8

## Salvamento de Arquivos

Arquivos salvos automaticamente em `data/output/`:
- Normal: `scene_name.ppm`
- Com soft shadows: `scene_name_soft.ppm`
- Com DOF: `scene_name_dof.ppm`

## Cenas de Exemplo

```bash
# Renderizar
./raytracer scene1.txt scene1.ppm 800 600
./raytracer scene_gallery.txt gallery.ppm 800 600
./raytracer scene_chess.txt chess.ppm 800 600
./raytracer scene_crystals.txt crystals.ppm 800 600
./raytracer scene_cityscape.txt city.ppm 800 600
```

**Cenas disponíveis:**
- `scene1.txt` - Cena do exemplo do TP
- `scene_gallery.txt` - Todas as formas geométricas
- `scene_chess.txt` - Tabuleiro de xadrez
- `scene_crystals.txt` - Cristais coloridos
- `scene_cityscape.txt` - Cidade

## Performance

Resoluções recomendadas:
- **Teste rápido**: 200x150 (~3-5s sem efeitos, ~10-15s com DOF)
- **Preview**: 400x300 (~15-25s sem efeitos, ~1-2min com DOF)
- **Alta qualidade**: 800x600 (~1-2min sem efeitos, ~4-8min com DOF)

**Impacto dos efeitos**:
- Soft Shadows (4 amostras): ~4x mais lento
- Depth of Field (8 amostras): ~8x mais lento
- Ambos combinados: ~32x mais lento

## Geometria Suportada

- **Esferas**
- **Cubos**
- **Prismas** (3-8+ lados: triangular, quadrado, pentagonal, hexagonal, octogonal...)
- **Pirâmides** (3-8+ lados)

## Materiais

- **Solid** - Cor sólida
- **Checker** - Padrão xadrez
- **Texmap** - Textura de imagem

## Organização de Arquivos

```
data/
├── scenes/      # Cenas (.txt)
├── textures/    # Texturas (.jpg, .ppm)
└── output/      # Renderizações (.ppm)
```