echo "=========================================="
echo "  20 Newsgroups Dataset Downloader"
echo "=========================================="
echo ""

DATASET_DIR="20news-18828"
DATASET_URL="http://qwone.com/~jason/20Newsgroups/20news-18828.tar.gz"
DATASET_FILE="20news-18828.tar.gz"

if [ -d "$DATASET_DIR" ]; then
    echo "Dataset directory '$DATASET_DIR' already exists."
    read -p "Do you want to download again? (y/n): " response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        echo "Skipping download. Using existing dataset."
        exit 0
    fi
    echo "Removing existing dataset..."
    rm -rf "$DATASET_DIR"
fi

if [ -f "$DATASET_FILE" ]; then
    echo "Archive '$DATASET_FILE' found. Extracting..."
    tar -xzf "$DATASET_FILE"
    if [ $? -eq 0 ]; then
        echo "Dataset extracted successfully!"
        echo "You can now run: ./search_engine $DATASET_DIR"
        exit 0
    else
        echo "Extraction failed. Will download again..."
        rm -f "$DATASET_FILE"
    fi
fi

echo "Downloading 20 Newsgroups dataset..."
if command -v wget &> /dev/null; then
    wget -c "$DATASET_URL" -O "$DATASET_FILE"
elif command -v curl &> /dev/null; then
    curl -L "$DATASET_URL" -o "$DATASET_FILE"
else
    echo "Error: Neither wget nor curl is installed."
    exit 1
fi

echo ""
echo "Extracting dataset..."
tar -xzf "$DATASET_FILE"

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "  Dataset downloaded and extracted!"
    echo "=========================================="
    echo ""
    echo "Dataset location: $DATASET_DIR"
    echo "Total files: $(find "$DATASET_DIR" -type f | wc -l)"
    echo ""
    
    read -p "Do you want to remove the archive file to save space? (y/n): " response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        rm -f "$DATASET_FILE"
        echo "Archive removed."
    fi
else
    echo "Error: Extraction failed."
    exit 1
fi