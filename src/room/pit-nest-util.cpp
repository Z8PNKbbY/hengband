#include "room/pit-nest-util.h"
#include "monster-race/monster-race-hook.h"
#include "monster-race/monster-race.h"
#include "system/dungeon-info.h"
#include "system/floor-type-definition.h"
#include "system/monster-race-info.h"
#include "util/enum-converter.h"
#include "util/probability-table.h"

/*!
 * @brief ダンジョン毎に指定されたピット配列を基準にランダムなnestタイプを決める
 * @param floor フロアへの参照
 * @param nest_types nest定義のマップ
 * @return 選択されたnestのID、選択失敗した場合nullopt.
 */
std::optional<NestKind> pick_nest_type(const FloorType &floor, const std::map<NestKind, nest_pit_type> &nest_types)
{
    ProbabilityTable<NestKind> table;
    for (const auto &[nest_kind, nest] : nest_types) {
        if (nest.level > floor.dun_level) {
            continue;
        }

        if (none_bits(floor.get_dungeon_definition().nest, (1UL << enum2i(nest_kind)))) {
            continue;
        }

        table.entry_item(nest_kind, nest.chance * MAX_DEPTH / (std::min(floor.dun_level, MAX_DEPTH - 1) - nest.level + 5));
    }

    if (table.empty()) {
        return std::nullopt;
    }

    return table.pick_one_at_random();
}

/*!
 * @brief ダンジョン毎に指定されたピット配列を基準にランダムなpitタイプを決める
 * @param floor フロアへの参照
 * @param pit_types pit定義のマップ
 * @return 選択されたpitのID、選択失敗した場合nullopt.
 */
std::optional<PitKind> pick_pit_type(const FloorType &floor, const std::map<PitKind, nest_pit_type> &pit_types)
{
    ProbabilityTable<PitKind> table;
    for (const auto &[pit_kind, pit] : pit_types) {
        if (pit.level > floor.dun_level) {
            continue;
        }

        if (none_bits(floor.get_dungeon_definition().pit, (1UL << enum2i(pit_kind)))) {
            continue;
        }

        table.entry_item(pit_kind, pit.chance * MAX_DEPTH / (std::min(floor.dun_level, MAX_DEPTH - 1) - pit.level + 5));
    }

    if (table.empty()) {
        return std::nullopt;
    }

    return table.pick_one_at_random();
}

/*!
 * @brief デバッグ時に生成されたpit/nestの型を出力する処理
 * @param type pit/nestの型ID
 * @param nest TRUEならばnest、FALSEならばpit
 * @return デバッグ表示文字列の参照ポインタ
 * @details
 * Hack -- Get the string describing subtype of pit/nest
 * Determined in prepare function (some pit/nest only)
 */
std::string pit_subtype_string(std::variant<NestKind, PitKind> type, bool nest)
{
    if (nest) {
        switch (std::get<0>(type)) {
        case NestKind::CLONE:
            return std::string("(").append(monraces_info[vault_aux_race].name).append(1, ')');
        case NestKind::SYMBOL_GOOD:
        case NestKind::SYMBOL_EVIL:
            return std::string("(").append(1, vault_aux_char).append(1, ')');
        }

        return "";
    }

    switch (std::get<1>(type)) {
    case PitKind::SYMBOL_GOOD:
    case PitKind::SYMBOL_EVIL:
        return std::string("(").append(1, vault_aux_char).append(1, ')');
    case PitKind::DRAGON:
        if (vault_aux_dragon_mask4.has_all_of({ MonsterAbilityType::BR_ACID, MonsterAbilityType::BR_ELEC, MonsterAbilityType::BR_FIRE, MonsterAbilityType::BR_COLD, MonsterAbilityType::BR_POIS })) {
            return _("(万色)", "(multi-hued)");
        }

        if (vault_aux_dragon_mask4.has(MonsterAbilityType::BR_ACID)) {
            return _("(酸)", "(acid)");
        }

        if (vault_aux_dragon_mask4.has(MonsterAbilityType::BR_ELEC)) {
            return _("(稲妻)", "(lightning)");
        }

        if (vault_aux_dragon_mask4.has(MonsterAbilityType::BR_FIRE)) {
            return _("(火炎)", "(fire)");
        }

        if (vault_aux_dragon_mask4.has(MonsterAbilityType::BR_COLD)) {
            return _("(冷気)", "(frost)");
        }

        if (vault_aux_dragon_mask4.has(MonsterAbilityType::BR_POIS)) {
            return _("(毒)", "(poison)");
        }

        return _("(未定義)", "(undefined)"); // @todo 本来は例外を飛ばすべき.
    default:
        return "";
    }
}
