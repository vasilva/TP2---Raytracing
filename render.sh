#!/bin/bash
# Script para renderizar cenas facilmente

# Uso: ./render.sh [scene_name] [width] [height]
#       ./render.sh --all [width] [height]
# Exemplo: ./render.sh scene_chess 400 300
#          ./render.sh --all 400 300

# Renderizar todas as cenas
if [ "$1" == "--all" ]; then
    WIDTH="${2:-800}"
    HEIGHT="${3:-600}"
    
    echo "=========================================="
    echo "Renderizando todas as cenas em ${WIDTH}x${HEIGHT}"
    echo "=========================================="
    echo ""
    
    TOTAL=0
    SUCCESS=0
    FAILED=0
    
    for SCENE_FILE in data/scenes/*.txt; do
        if [ -f "$SCENE_FILE" ]; then
            SCENE=$(basename "$SCENE_FILE" .txt)
            OUTPUT="${SCENE}.ppm"
            
            echo "[$((TOTAL+1))] Renderizando ${SCENE}..."
            ./raytracer "${SCENE}.txt" "${OUTPUT}" "${WIDTH}" "${HEIGHT}" > /dev/null 2>&1
            
            if [ $? -eq 0 ]; then
                echo "  ✓ Sucesso: data/output/${OUTPUT}"
                SUCCESS=$((SUCCESS+1))
            else
                echo "  ✗ Falhou: ${SCENE}"
                FAILED=$((FAILED+1))
            fi
            TOTAL=$((TOTAL+1))
            echo ""
        fi
    done
    
    echo "=========================================="
    echo "Resumo: ${SUCCESS}/${TOTAL} renderizações concluídas"
    if [ $FAILED -gt 0 ]; then
        echo "        ${FAILED} falharam"
    fi
    echo "=========================================="
    exit 0
fi

# Renderizar uma cena específica
if [ -z "$1" ]; then
    echo "Uso: $0 <scene_name> [width] [height]"
    echo "      $0 --all [width] [height]"
    echo ""
    echo "Cenas disponíveis:"
    ls data/scenes/*.txt 2>/dev/null | xargs -n1 basename | sed 's/.txt$//'
    exit 1
fi

SCENE="$1"
WIDTH="${2:-800}"
HEIGHT="${3:-600}"

# Remove .txt se foi incluído
SCENE="${SCENE%.txt}"

# Nome do arquivo de saída
OUTPUT="${SCENE}.ppm"

echo "Renderizando ${SCENE} em ${WIDTH}x${HEIGHT}..."
./raytracer "${SCENE}.txt" "${OUTPUT}" "${WIDTH}" "${HEIGHT}"

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Renderização completa!"
    echo "  Entrada: data/scenes/${SCENE}.txt"
    echo "  Saída: data/output/${OUTPUT}"
else
    echo ""
    echo "✗ Erro na renderização"
fi
