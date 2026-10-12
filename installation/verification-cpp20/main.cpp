// Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:29:41
// Programme de vérification de l'environnement : il ne compile que si le compilateur
// prend réellement en charge C++20 (concepts, ranges, format, span, <=>, consteval…).
#include <algorithm>
#include <compare>
#include <concepts>
#include <format>
#include <iostream>
#include <numeric>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace {

template <typename T>
concept Numerique = std::integral<T> || std::floating_point<T>;

template <Numerique T>
T somme(std::span<const T> valeurs) {
    return std::accumulate(valeurs.begin(), valeurs.end(), T{});
}

struct Version {
    int majeure;
    int mineure;
    auto operator<=>(const Version&) const = default;  // comparaison à trois voies
};

consteval int carre(int n) { return n * n; }  // évalué à la compilation

}  // namespace

int main() {
    const std::vector<int> nombres{1, 2, 3, 4, 5, 6, 7, 8};
    auto pairsAuCarre = nombres | std::views::filter([](int n) { return n % 2 == 0; })
                                | std::views::transform([](int n) { return n * n; });
    const std::vector<int> resultat(pairsAuCarre.begin(), pairsAuCarre.end());

    const Version minimum{.majeure = 19, .mineure = 29};  // initialiseurs désignés
    const Version compilateur{.majeure = _MSC_VER / 100, .mineure = _MSC_VER % 100};

    std::cout << std::format("C++20 OK : MSVC {}.{} (_MSC_VER {}), _MSVC_LANG {}\n", compilateur.majeure,
                             compilateur.mineure, _MSC_VER, _MSVC_LANG);
    std::cout << std::format("somme = {}, carrés pairs = {}, carre(12) = {}\n",
                             somme(std::span<const int>(nombres)), resultat.size(), carre(12));

    const bool conforme = compilateur >= minimum && _MSVC_LANG >= 202002L && resultat == std::vector<int>{4, 16, 36, 64};
    std::cout << (conforme ? "Environnement C++20 conforme\n" : "Environnement NON conforme\n");
    return conforme ? 0 : 1;
}
