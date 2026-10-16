// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
// Test de charge Gatling (Java DSL) — exigence EX-13 :
//   un coup est traité en moins de 50 ms pour 95 % des requêtes, avec 50 parties jouées simultanément.
// Même scénario que le script k6 : chaque utilisateur virtuel crée une partie et la joue jusqu'à la victoire.
package puissance4;

import static io.gatling.javaapi.core.CoreDsl.*;
import static io.gatling.javaapi.http.HttpDsl.*;

import io.gatling.javaapi.core.ScenarioBuilder;
import io.gatling.javaapi.core.Simulation;
import io.gatling.javaapi.http.HttpProtocolBuilder;

import java.time.Duration;
import java.util.List;

public class ChargeCoupsSimulation extends Simulation {

    private static final String URL = System.getProperty("p4.url", "http://127.0.0.1:8080");
    private static final List<Integer> VICTOIRE_VERTICALE = List.of(0, 1, 0, 1, 0, 1, 0);

    private final HttpProtocolBuilder protocole = http
            .baseUrl(URL)
            .contentTypeHeader("application/json")
            .acceptHeader("application/json");

    // Modèle « fermé » comme k6 : 50 utilisateurs qui enchaînent les parties pendant le palier,
    // chacun sur SA connexion HTTP (réutilisée). Remplacer chaque utilisateur par un nouveau
    // ouvrirait une connexion TCP par partie et épuiserait les ports du poste : on mesurerait l'injecteur.
    private final ScenarioBuilder partie = scenario("Jouer des parties jusqu'à la victoire")
            .exec(session -> session.set("numero", session.userId()))  // noms de joueurs uniques
            .during(Duration.ofSeconds(40)).on(
                    exec(http("creation")
                            .post("/api/parties")
                            .body(StringBody("{\"joueur1\":\"Alice-#{numero}\",\"joueur2\":\"Bob-#{numero}\"}"))
                            .check(status().is(201), jsonPath("$.id").saveAs("idPartie")))
                    .exitHereIfFailed()
                    .foreach(VICTOIRE_VERTICALE, "colonne").on(
                            exec(http("coup")
                                    .post("/api/parties/#{idPartie}/coups")
                                    .body(StringBody("{\"colonne\":#{colonne}}"))
                                    .check(status().is(200)))));

    {
        setUp(partie
                .injectOpen(rampUsers(50).during(Duration.ofSeconds(10))))  // montée en charge jusqu'à 50 parties (EX-13)
                .protocols(protocole)
                .assertions(  // critères d'acceptation : la simulation ÉCHOUE s'ils ne sont pas tenus
                        details("coup").responseTime().percentile(95.0).lt(50),
                        global().failedRequests().percent().lt(1.0));
    }
}
