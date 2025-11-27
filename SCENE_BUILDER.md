# Scene Builder - Guia Rápido

Ferramenta para criar cenas de forma interativa ou gerar exemplos prontos.

## Uso Básico

```bash
# Modo interativo
python3 scene_builder.py

# Gerar exemplo
python3 scene_builder.py -e gallery

# Gerar todas as 6 cenas
python3 scene_builder.py --all
```

## Cenas de Exemplo

| Comando | Cena | Descrição |
|---------|------|-----------|
| `-e gallery` | `scene_gallery.txt` | Todas as formas geométricas |
| `-e solar` | `scene_solar_system.txt` | Sistema solar |
| `-e chess` | `scene_chess.txt` | Tabuleiro de xadrez |
| `-e temple` | `scene_temple.txt` | Templo com colunas |
| `-e crystals` | `scene_crystals.txt` | Cristais coloridos |
| `-e city` | `scene_cityscape.txt` | Paisagem urbana |

## Comandos Interativos

### Câmera
```
camera <px> <py> <pz> <tx> <ty> <tz> <nx> <ny> <nz> <fov>
```
Padrão: `camera 0 30 -200 0 10 -100 0 1 0 40`

### Luz
```
light <x> <y> <z> [r g b] [rho0 rho1 rho2]
```
Exemplos:
- `light 100 100 -100` - Luz branca
- `light 50 80 -50 1 0.8 0.8` - Luz avermelhada

### Pigmentos

```
solid <r> <g> <b>
checker <r1> <g1> <b1> <r2> <g2> <b2> <size>
texmap <filename>
```

Exemplos:
- `solid 1 0 0` → ID 0 (vermelho)
- `checker 0.1 0.1 0.1 0.9 0.9 0.9 20` → Xadrez preto/branco
- `texmap texture1.ppm` → Textura

### Materiais (Finishes)

```
finish <ka> <kd> <ks> <alpha> <kr> <kt> <ior>
```

Presets úteis:
- **Mate**: `finish 0.3 0.7 0.0 1 0.0 0.0 0.0`
- **Brilhante**: `finish 0.2 0.5 0.3 50 0.3 0.0 0.0`
- **Espelho**: `finish 0.1 0.1 0.9 1000 0.9 0.0 0.0`

### Objetos

```
sphere <x> <y> <z> <radius> <pig_id> <fin_id>
cube <x> <y> <z> <size> <pig_id> <fin_id>
box <x> <y> <z> <width> <height> <depth> <pig_id> <fin_id>
prism <x> <y> <z> <radius> <height> <sides> <pig_id> <fin_id>
pyramid <x> <y> <z> <radius> <height> <sides> <pig_id> <fin_id>
```

**Prismas e Pirâmides aceitam 3-8+ lados:**
- 3 = Triangular
- 4 = Quadrado
- 5 = Pentagonal
- 6 = Hexagonal
- 8 = Octogonal

### Utilitários

```
list            # Mostra resumo da cena
save <nome>     # Salva em data/scenes/<nome>.txt
quit            # Sair
```

## Exemplo de Sessão

```bash
$ python3 scene_builder.py

> solid 1 0 0
Solid pigment added (id: 0)

> solid 0 0 1
Solid pigment added (id: 1)

> finish 0.3 0.7 0 1 0 0 0
Surface finish added (id: 0)

> light 100 100 -100
Light added (total: 1)

> sphere 0 10 0 15 0 0
Sphere added (total objects: 1)

> prism -25 10 0 10 20 6 1 0
Hexagonal prism added (total objects: 2)

> pyramid 25 0 0 12 25 5 0 0
Pentagonal pyramid added (total objects: 3)

> list
Current Scene:
  Camera: pos=(0, 30, -200), target=(0, 10, -100), fov=40
  Lights: 1
  Pigments: 2
  Finishes: 1
  Objects: 3

> save minha_cena
Scene saved to data/scenes/minha_cena.txt

> quit
```

## Renderizar Cena Criada

```bash
./raytracer minha_cena.txt output.ppm 400 300
```

O arquivo será procurado em `data/scenes/` e salvo em `data/output/`.

## Dicas

1. **Plano de chão**: Esfera gigante abaixo
   ```
   sphere 0 -1000 0 1000 <pig> <fin>
   ```

2. **IDs começam em 0**: Primeiro pigmento = ID 0, segundo = ID 1, etc.

3. **Radius em polígonos**: É a distância do centro ao vértice, não o lado.

4. **Testar rapidamente**: Renderize em 200x150 para preview (~5s)

## Formas Geométricas

### Prismas (bases regulares)
```
prism 0 15 0 10 20 3 0 0  # Triangular
prism 0 15 0 10 20 4 0 0  # Quadrado
prism 0 15 0 10 20 5 0 0  # Pentagonal
prism 0 15 0 10 20 6 0 0  # Hexagonal
prism 0 15 0 10 20 8 0 0  # Octogonal
```

### Pirâmides (bases regulares)
```
pyramid 0 0 0 12 25 3 0 0  # Tetraedro
pyramid 0 0 0 12 25 4 0 0  # Pirâmide quadrada (Egito)
pyramid 0 0 0 12 25 5 0 0  # Pentagonal
pyramid 0 0 0 12 25 6 0 0  # Hexagonal
```

## Parâmetros de Material

- `ka`: Ambiente (0-1) - quão iluminado sem luz direta
- `kd`: Difuso (0-1) - espalhamento da luz
- `ks`: Especular (0-1) - brilho/reflexo especular
- `alpha`: Brilho (1-1000+) - tamanho do ponto de luz
- `kr`: Reflexão (0-1) - espelho
- `kt`: Transmissão (0-1) - transparência
- `ior`: Índice refração (1.0=ar, 1.5=vidro)

## Arquivos Gerados

Todas as cenas são salvas em `data/scenes/` no formato esperado pelo raytracer.

Estrutura do arquivo:
```
<camera: 4 linhas>
<n_lights>
<lights: n_lights linhas>
<n_pigments>
<pigments: n_pigments blocos>
<n_finishes>
<finishes: n_finishes linhas>
<n_objects>
<objects: n_objects blocos>
```
