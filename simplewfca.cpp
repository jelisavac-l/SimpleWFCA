#include <iostream>
#include <vector>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iomanip>

#define RED "\u001b[31m"
#define GRN "\u001b[32m"
#define YEL "\u001b[33m"
#define BLU "\u001b[34m"
#define RESET "\u001b[0m"
#define SPEED 10ms
#define MAP_SIZE_X 32
#define MAP_SIZE_Y 56
static int collapsed_tiles = 0;
// #define SEA_CHAR '\u001b[34m S\u001b[0m'
// #define COAST_CHAR '\u001b[33m C\u001b[0m'
// #define LAND_CHAR '\u001b[32m L\u001b[0m'
#define WIP_CHAR "█"
#define SEA_CHAR "█"
#define COAST_CHAR "█"
#define LAND_CHAR "█"
#define UNKNOWN_CHAR '?'

using namespace std;

enum TYPE {UNKNOWN, WIP, SEA, COAST, LAND};
typedef struct
{
    vector<TYPE> valid_types;
    int entropy;
    TYPE tile_type = UNKNOWN;

}TILE;

int get_random_int(int min, int max);
void clear_screen();
void render(TILE map[MAP_SIZE_X][MAP_SIZE_Y]);
void init_map(TILE map[MAP_SIZE_X][MAP_SIZE_Y]);
// Provide map for rendering, tile to be updated and its new type
void update_tile(TILE map[MAP_SIZE_X][MAP_SIZE_Y], TILE* tile, TYPE type);
// Returns a pointer to a TILE with smallest entropy
TILE* update_tile_entropy(TILE map[MAP_SIZE_X][MAP_SIZE_Y], TILE* first);
void collapse(TILE map[MAP_SIZE_X][MAP_SIZE_Y]);    // Uf...

int main()
{
    TILE map[MAP_SIZE_X][MAP_SIZE_Y];
    init_map(map);
    render(map);
    
    // // Testing update tile
    // for(size_t i = 0; i < MAP_SIZE_X; i++)
    //     for (size_t j = 0; j < MAP_SIZE_Y; j++)
    //     {
    //         srand(time(NULL));
    //         int randNum = (rand() * 1.0 / RAND_MAX * 3) + 2;
    //         update_tile(map, &map[i][j], (TYPE)randNum);
    //     }
    collapse(map);
    return 0;
}

int get_random_int(int min, int max)
{
    srand(time(NULL));
    return rand() * 1.0 / RAND_MAX * (max-min+1) + min;
}

void clear_screen()
{
    cout << "\033[2J\033[1;1H";
}

void render(TILE map[MAP_SIZE_X][MAP_SIZE_Y])
{
    for(size_t i = 0; i < MAP_SIZE_X; i++) {
        for (size_t j = 0; j < MAP_SIZE_Y; j++) {
            switch (map[i][j].tile_type)
            {
            case UNKNOWN:
                cout << map[i][j].entropy;
                break;
            case WIP:
                cout << RED << WIP_CHAR << RESET;
                break;
            case SEA:
                cout << BLU << map[i][j].entropy << RESET;
                break;
            case COAST:
                cout << YEL << map[i][j].entropy << RESET;
                break;
            case LAND:
                cout << GRN << map[i][j].entropy << RESET;
                break;
            }
        }
        if(i == 15) cout << "\tSimple Wave Function";
        if(i == 16) cout << "\tCollapse Algorithm";
        if(i == 17) cout << "\t(c) Luka Jelisavac";
        if(i == 18) cout << "\t" << setfill('0') << setw(4) << collapsed_tiles << " / " << MAP_SIZE_X * MAP_SIZE_Y;
        cout << endl;
    }
        
}

void init_map(TILE map[MAP_SIZE_X][MAP_SIZE_Y])
{
    for(size_t i = 0; i < MAP_SIZE_X; i++)
        for (size_t j = 0; j < MAP_SIZE_Y; j++) {
            map[i][j].tile_type = UNKNOWN;
            map[i][j].entropy = 3;
            map[i][j].valid_types.push_back(SEA);
            map[i][j].valid_types.push_back(COAST);
            map[i][j].valid_types.push_back(LAND);
        }
}

void update_tile(TILE map[MAP_SIZE_X][MAP_SIZE_Y], TILE *tile, TYPE type)
{
    tile->tile_type = WIP;
    clear_screen();
    render(map);
    this_thread::sleep_for(SPEED);
    tile->tile_type = type;
    clear_screen();
    render(map);
}

TILE* update_tile_entropy(TILE map[MAP_SIZE_X][MAP_SIZE_Y], TILE* first)
{

    TILE* smallest_entropy = NULL;

    for(size_t i = 0; i < MAP_SIZE_X; i++)
        for (size_t j = 0; j < MAP_SIZE_Y; j++){
            if (map[i][j].tile_type == UNKNOWN)
            {
                // Specify collapse rules
                if(j == 0 )  // SEGFAULT
                {
                    map[i][j].valid_types.clear();
                    map[i][j].valid_types.push_back(LAND);
                    map[i][j].valid_types.push_back(COAST);
                }

                //LAND LEFT
                else if(map[i][j-1].tile_type == LAND){
                    map[i][j].valid_types.clear();
                    map[i][j].valid_types.push_back(LAND);
                    map[i][j].valid_types.push_back(COAST);
                }
                
                // SEA/COAST LEFT
                else if(map[i][j-1].tile_type == SEA || map[i][j-1].tile_type == COAST) {
                    map[i][j].valid_types.clear();
                    map[i][j].valid_types.push_back(SEA);
                }

                if(j == MAP_SIZE_Y - 1)
                {
                    // ...
                }

                else if(map[i][j+1].tile_type == LAND){
                    map[i][j].valid_types.clear();
                    map[i][j].valid_types.push_back(LAND);
                }
                

                map[i][j].entropy = map[i][j].valid_types.size();
                // Initial minimum entropy
                if(smallest_entropy == NULL) smallest_entropy = &map[i][j];
                if(smallest_entropy->entropy >= map[i][j].entropy && map[i][j].entropy > 0)
                {
                    smallest_entropy = &map[i][j];
                }
            }
            else map[i][j].entropy = 0;
        }

        
        return smallest_entropy;
}

void collapse(TILE map[MAP_SIZE_X][MAP_SIZE_Y])
{

    // Collapse a random tile to final state
    int firstX = get_random_int(0, MAP_SIZE_X-1);
    int firstY = get_random_int(0, MAP_SIZE_Y-1);
    TYPE firstType = (TYPE)get_random_int(2, 4);
    update_tile(map, &map[firstX][firstY], firstType);
    map[firstX][firstY].entropy = 0;
    collapsed_tiles++;

    // Testing smallest entropy
    while(collapsed_tiles < MAP_SIZE_X * MAP_SIZE_Y)
    {
        TILE* sm_ent = update_tile_entropy(map, &map[firstX][firstY]);
        int random_type_index = get_random_int(0, sm_ent->valid_types.size()-1);
        TYPE random_type = sm_ent->valid_types.at(random_type_index);
        update_tile(map, sm_ent, (TYPE)random_type);
        collapsed_tiles++;
    }
    

}
