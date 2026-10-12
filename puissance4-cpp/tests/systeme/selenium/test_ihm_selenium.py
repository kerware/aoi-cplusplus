# Dernière génération/modification faite par l'IA Claude le 09/10/2026 18:15:03
"""Tests d'IHM avec Selenium WebDriver (Python + pytest), pour comparaison avec Playwright.

    pip install selenium pytest
    $env:P4_URL = "http://127.0.0.1:8080"; python -m pytest tests/systeme/selenium -v

Selenium Manager (inclus dans Selenium 4) trouve Edge et télécharge le pilote adapté.
Différences avec Playwright à observer : attentes explicites à écrire soi-même (WebDriverWait),
pas d'attente automatique sur les assertions, pas de trace intégrée.
"""
import os

import pytest
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as ec
from selenium.webdriver.support.ui import WebDriverWait

URL = os.environ.get("P4_URL", "http://127.0.0.1:8080")


def par_testid(valeur):
    return By.CSS_SELECTOR, f'[data-testid="{valeur}"]'


@pytest.fixture
def navigateur():
    options = webdriver.EdgeOptions()
    options.add_argument("--headless=new")
    pilote = webdriver.Edge(options=options)
    yield pilote
    pilote.quit()


def creer_partie(navigateur, joueur1, joueur2):
    navigateur.get(URL)
    navigateur.find_element(*par_testid("joueur1")).send_keys(joueur1)
    navigateur.find_element(*par_testid("joueur2")).send_keys(joueur2)
    navigateur.find_element(*par_testid("nouvelle-partie")).click()


def test_une_nouvelle_partie_affiche_alice_comme_joueur_courant(navigateur):
    creer_partie(navigateur, "Alice", "Bob")

    # Attente EXPLICITE : sans elle, le test lit la page avant la réponse du serveur (test instable)
    WebDriverWait(navigateur, 5).until(ec.text_to_be_present_in_element(par_testid("joueur-courant"), "Alice"))
    assert len(navigateur.find_elements(By.CSS_SELECTOR, ".case")) == 42


def test_jouer_un_coup_fait_tomber_le_jeton_et_passe_la_main(navigateur):
    creer_partie(navigateur, "Alice", "Bob")
    attente = WebDriverWait(navigateur, 5)
    attente.until(ec.element_to_be_clickable(par_testid("colonne-3"))).click()

    attente.until(ec.text_to_be_present_in_element(par_testid("joueur-courant"), "Bob"))
    assert "R" in navigateur.find_element(*par_testid("case-5-3")).get_attribute("class").split()
