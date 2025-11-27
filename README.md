# Raytracer - Guia Rápido

Ray tracer com distributed ray tracing (soft shadows, depth of field, motion blur).

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
- `3` - Motion Blur (desfoque de movimento)

## Distributed Ray Tracing

### Soft Shadows (Tecla 1)
Sombras com bordas suaves usando 4 amostras por luz.

### Depth of Field (Tecla 2)
Objetos fora de foco desfocados. Parâmetros:
- Abertura: 0.5
- Distância focal: 150.0
- Amostras: 8

### Motion Blur (Tecla 3)
Objetos em movimento aparecem desfocados (objetos com y < 0 se movem automaticamente).
- Tempo: 0.5s
- Amostras: 4

## Salvamento de Arquivos

Arquivos salvos automaticamente em `data/output/`:
- Normal: `scene_name.ppm`
- Com soft shadows: `scene_name_soft.ppm`
- Com DOF: `scene_name_dof.ppm`
- Com blur: `scene_name_blur.ppm`

## Cenas de Exemplo

```bash
# Gerar todas as cenas
python3 scene_builder.py --all

# Renderizar
./raytracer scene_gallery.txt gallery.ppm 400 300
./raytracer scene_solar_system.txt solar.ppm 400 300
./raytracer scene_chess.txt chess.ppm 400 300
./raytracer scene_temple.txt temple.ppm 400 300
./raytracer scene_crystals.txt crystals.ppm 400 300
./raytracer scene_cityscape.txt city.ppm 400 300
```

**Cenas disponíveis:**
- `scene_gallery.txt` - Todas as formas geométricas
- `scene_solar_system.txt` - Sistema solar
- `scene_chess.txt` - Tabuleiro de xadrez
- `scene_temple.txt` - Templo com colunas
- `scene_crystals.txt` - Cristais coloridos
- `scene_cityscape.txt` - Cidade
- `scene_aspect_test.txt` - Teste de aspect ratio

## Performance

Resoluções recomendadas:
- Teste: 200x150 (~5s)
- Preview: 400x300 (~20s)
- Final: 800x600 (~1-2min)

Com efeitos distribuídos: 2-4x mais lento.

## Geometria Suportada

- **Esferas**
- **Cubos**
- **Prismas** (3-8+ lados: triangular, quadrado, pentagonal, hexagonal, octogonal...)
- **Pirâmides** (3-8+ lados)

## Materiais

- **Solid** - Cor sólida
- **Checker** - Padrão xadrez
- **Texmap** - Textura de imagem

## Aspect Ratio

O raytracer corrige automaticamente o aspect ratio. Esferas aparecem sempre redondas.

Para testar:
```bash
./raytracer scene_aspect_test.txt test_square.ppm 800 800
./raytracer scene_aspect_test.txt test_rect.ppm 800 600
./raytracer scene_aspect_test.txt test_wide.ppm 1600 600
```

As esferas devem aparecer redondas em todas.

## Organização de Arquivos

```
data/
├── scenes/      # Cenas (.txt)
├── textures/    # Texturas (.jpg, .ppm)
└── output/      # Renderizações (.ppm)
```
