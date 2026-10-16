# Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:27:13
# ---------------------------------------------------------------------------
# Jenkins de la formation AOI (chapitre 5) : Jenkins LTS + plugins + client Docker.
# Les builds s'exécutent dans des conteneurs (docker build, docker compose) lancés par le
# démon Docker de l'hôte (socket monté) : aucun compilateur n'est installé dans Jenkins.
# ---------------------------------------------------------------------------
FROM jenkins/jenkins:2.580.1-lts-jdk21

USER root
# Client Docker + plugins buildx et compose (dépôt officiel Docker), Python 3 pour les scripts ci/
RUN apt-get update \
 && apt-get install -y --no-install-recommends ca-certificates curl gnupg python3 \
 && install -m 0755 -d /etc/apt/keyrings \
 && curl -fsSL https://download.docker.com/linux/debian/gpg -o /etc/apt/keyrings/docker.asc \
 && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/debian $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
      > /etc/apt/sources.list.d/docker.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends docker-ce-cli docker-buildx-plugin docker-compose-plugin \
 && rm -rf /var/lib/apt/lists/* \
 && git config --system --add safe.directory '*'
USER jenkins

# Plugins (versions compatibles résolues par jenkins-plugin-cli) et configuration « as code »
COPY plugins.txt /usr/share/jenkins/ref/plugins.txt
RUN jenkins-plugin-cli --plugin-file /usr/share/jenkins/ref/plugins.txt
COPY casc.yaml /usr/share/jenkins/ref/casc.yaml
ENV CASC_JENKINS_CONFIG=/usr/share/jenkins/ref/casc.yaml
# Pas d'assistant d'installation : tout est décrit dans casc.yaml.
# ALLOW_LOCAL_CHECKOUT : le dépôt Git du participant est monté dans le conteneur (file:///depots/...).
ENV JAVA_OPTS="-Djenkins.install.runSetupWizard=false -Dhudson.plugins.git.GitSCM.ALLOW_LOCAL_CHECKOUT=true"
