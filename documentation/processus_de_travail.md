## Processus de travail 

### Répartition des tâches
Les tâches sont également divisées et réparties sur Trello pour avoir une vue d'ensemble
de l'avancement du projet.

#### Semaine 1
- Landing page: Benjamin
- Pipeline DevOps: Nathan
- Description du projet et spécifications: Colin
- Processus de travail: Pierric

#### Semaine 2 et 3
- Design de l'app desktop (QT):
- Backend de l'app desktop (BlockChain): Nathan
- Site web: Benjamin

### Organisation du repo & DevOps

* 3 branches parallèles seront utilisées pour le développement:
    * dev_doc : Documentation, rendus PDG et présentation
    * dev_app : Développement de l'application C++ et Qt
    * dev_web : Site internet
    * main : seules les pull requests ayant passé les tests sont déployées sur main. 
    la branche contient également l'application buildée correspondant au code présent sur main
* Protection de la branche main
    * Pull requests obligatoires
    * Commits signés (vérifiés) obligatoires
* Utilisation des pipelines github (actions)
    * Une pipeline d'intégration des changements
        * Vérifie le code (lint)
        * Effectue les tests
        * Build et déploie sur main
    * Une pipeline de release
        * Créé une release github avec les artéfacts buildés présents sur main
        * Augmente le numéro de version
        * Le site web doit pointer sur les artéfacts de la dernière release