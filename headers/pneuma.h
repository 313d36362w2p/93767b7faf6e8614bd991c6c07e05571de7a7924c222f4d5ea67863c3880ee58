// CRC Table Entry
typedef struct {
    uint32_t crc;
    char symbol;
    size_t padding;
} CRC_Entry;

// Mod Info Table Entry
typedef struct {
    char key;
    char value;
    size_t entry_size;
} MI_Entry;
