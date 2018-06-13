#include "Cell.hpp"
#include "JsonError.hpp"

#include "nlohmann/json.hpp"

/** The JSON key for cell type. */
constexpr auto JSON_TYPE_KEY = "type";
/** The JSON key for production. */
constexpr auto JSON_PRODUCTION_KEY = "production";
/** The JSON key for energy factor. */
constexpr auto JSON_ENERGY_FACTOR_KEY = "energy_factor";

namespace hlt {

/**
 * Convert a Cell to JSON format.
 * @param[out] json The output JSON.
 * @param cell The cell to convert.
 */
void to_json(nlohmann::json &json, const Cell &cell) { cell->to_json(json); }

/**
 * Convert an encoded Cell from JSON format.
 * @param json The JSON.
 * @param[out] cell The converted cell.
 */
void from_json(const nlohmann::json &json, Cell &cell) {
    // The type field determines the Cell subclass that will be instantiated.
    const auto &type = json.at(JSON_TYPE_KEY).get<std::string>();
    if (type == NormalCell::CELL_TYPE_NAME) {
        cell = std::make_unique<NormalCell>(json);
    } else if (type == ObstacleCell::CELL_TYPE_NAME) {
        cell = std::make_unique<ObstacleCell>(json);
    } else if (type == EnergyFactorCell::CELL_TYPE_NAME) {
        cell = std::make_unique<EnergyFactorCell>(json);
    } else if (type == FactoryCell::CELL_TYPE_NAME) {
        cell = std::make_unique<FactoryCell>(json);
    } else {
        throw JsonError(json);
    }
}

/**
 * Write a Cell to bot serial format.
 * @param ostream The output stream.
 * @param cell The cell to write.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &ostream, const Cell &cell) {
    return ostream << cell->to_bot_serial() << std::endl;
}

/**
 * Create ProductionCell from JSON.
 * @param json The JSON.
 */
ProductionCell::ProductionCell(const nlohmann::json &json) :
        _production(json.at(JSON_PRODUCTION_KEY).get<decltype(ProductionCell::_production)>()) {}

constexpr char const *NormalCell::CELL_TYPE_NAME;

/**
 * Convert a NormalCell to JSON format.
 * @param[out] json The JSON output.
 */
void NormalCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,       CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY, production()}};
}

/**
 * Convert a NormalCell to bot serial format.
 * @return The formatted output.
 */
std::string NormalCell::to_bot_serial() const {
    return std::string(CELL_TYPE_NAME) + " " + std::to_string(production());
}

constexpr char const *ObstacleCell::CELL_TYPE_NAME;

/**
 * Convert an ObstacleCell to JSON format.
 * @param[out] json The JSON output.
 */
void ObstacleCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,       CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY, production()}};
}

/**
 * Convert a ObstacleCell to bot serial format.
 * @return The formatted output.
 */
std::string ObstacleCell::to_bot_serial() const {
    return std::string(CELL_TYPE_NAME) + " " + std::to_string(production());
}

constexpr char const *EnergyFactorCell::CELL_TYPE_NAME;

/**
 * Convert an EnergyFactorCell to JSON format.
 * @param[out] json The JSON output.
 */
void EnergyFactorCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY,          CELL_TYPE_NAME},
            {JSON_PRODUCTION_KEY,    production()},
            {JSON_ENERGY_FACTOR_KEY, _energy_factor}};
}

/**
 * Convert a EnergyFactorCell to bot serial format.
 * @return The formatted output.
 */
std::string EnergyFactorCell::to_bot_serial() const {
    return std::string(JSON_ENERGY_FACTOR_KEY) + " " +
           std::to_string(production()) + " " +
           std::to_string(_energy_factor);
}

/**
 * Create EnergyFactorCell from JSON.
 * @param json The JSON.
 */
EnergyFactorCell::EnergyFactorCell(const nlohmann::json &json) :
        ProductionCell(json),
        _energy_factor(json.at(CELL_TYPE_NAME).get<decltype(EnergyFactorCell::_energy_factor)>()) {}

constexpr char const *FactoryCell::CELL_TYPE_NAME;

/**
 * Convert a FactoryCell to JSON format.
 * @param[out] json The JSON output.
 */
void FactoryCell::to_json(nlohmann::json &json) const {
    json = {{JSON_TYPE_KEY, CELL_TYPE_NAME}};
}

/**
 * Convert a FactoryCell to bot serial format.
 * @return The formatted output.
 */
std::string FactoryCell::to_bot_serial() const { return CELL_TYPE_NAME; }

}