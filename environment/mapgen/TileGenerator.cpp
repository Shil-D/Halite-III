#include "TileGenerator.hpp"
#include "Map.hpp"

#include <assert.h>

namespace hlt {
    namespace mapgen {

        hlt::Map TileGenerator::tile_map(const Map::dimension_type factory_y, const Map::dimension_type factory_x,
                                const hlt::Map &tile, std::list<hlt::Player> &players) {
            auto map = Map(width, height);
            // Copy the tile over the map
            for (long player_row = 0; player_row < num_tile_rows; ++player_row) {
                for (long player_col = 0; player_col < num_tile_cols; ++player_col) {
                    for (long tile_row = 0; tile_row < tile_height; ++tile_row) {
                        for (long tile_col = 0; tile_col < tile_width; ++tile_col) {
                            map.grid[player_row * tile_height + tile_row][player_col * tile_width + tile_col] =
                                    std::make_unique<NormalCell>(tile.grid[tile_row][tile_col] -> production());
                        }
                    }
                }
            }

            // Place a factory for each player on the map at corresponding relative locations and update each
            // player to know their factory's location
            long player_idx = 0;
            for (auto &player : players) {
                const auto player_factory_x = (player_idx % num_tile_cols) * tile_width + factory_x;
                const auto player_factory_y = (player_idx / num_tile_cols) * tile_height + factory_y;
                map.grid[player_factory_y][player_factory_x] = std::make_unique<FactoryCell>();

                hlt::Location factory_location {(long) player_factory_x, (long) player_factory_y};
                player.factory_location = factory_location;

                player_idx++;
            }

            return map;

        }

        TileGenerator::TileGenerator(const MapParameters &parameters) :
                Generator(parameters),
                num_players(parameters.num_players),
                width(parameters.width),
                height(parameters.height){

            // Ensure that the map can be subdivided into partitions for a given number of players
            // ie: a 64x64 map cannot be (most basic definition of) symmetrical for 6 players
            // TODO: find a way to dea with cases when this is not true that's less disruptive than assertion errors - KB
            assert((width * height) % num_players == 0);

            // We want our map to be as square-like as possible, so to find the size of a tile, first determine
            // how many tiles we will have
            num_tile_rows = (unsigned long) sqrt(num_players);
            while (num_players % num_tile_rows != 0) {
                num_tile_rows--;
            }
            num_tile_cols = num_players / num_tile_rows;

            // next, use number of rows and cols of player start tiles to determine height and width of a single tile
            tile_width = width / num_tile_cols;
            tile_height = height / num_tile_rows;

            // Ensure these tiles then make up the whole map
            assert(tile_width * num_tile_cols == width && tile_height * num_tile_rows == height);

        }

    }
}