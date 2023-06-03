# Parser NMEA

## Préambule

Le but de ce projet est de créer un mini parser NMEA en C ou C++ (le C++ étant **fortement** recommandé).

La norme NMEA est un format de données textuelles qui permet de représenter des données de géolocalisation. Ces données sont envoyées par des capteurs GPS, et sont utilisées par des logiciels de navigation. Le format NMEA est un format standardisé, et est utilisé par la plupart des capteurs GPS.

Ici, nous nous intéresserons au standard NMEA 0183, qui est le plus répandu.

## Messages à interpréter

Par soucis de simplicité, nous nous intéresserons uniquement aux messages de type `GGA`. Si le message n'est pas de type `GGA`, alors vous pouvez simplement écrire un message d'erreur sur la sortie d'erreur, indiquant par exemple que ce type de message n'est pas supporté.

## Objectifs

Le but de ce projet est de pouvoir interpréter quelques messages NMEA, et de les afficher sur la sortie standard.

Les messages seront écrits dans un fichier texte, et votre programme devra les lire et afficher ce que vous estimez être pertinent sur la sortie standard.

Le but de cet exercice est surtout d'évaluer votre capacité à écrire un programme qui soit facilement extensible. Ainsi, vous devrez écrire votre programme de manière à ce qu'il soit facile d'ajouter de nouveaux types de messages à interpréter. En effet, le parsing du `GGA` est en réalité secondaire, et vous devrez surtout vous concentrer sur la manière dont vous allez écrire votre programme.

Les consignes sont volontairement peu précises, et vous êtes libres de faire comme bon vous semble. Votre bon sens et votre capacité à prendre des décisions seront évalués.

## Exemple de messages

Voici un fichier texte contenant quelques messages NMEA. Vous pouvez vous en servir pour tester votre programme.

```txt
$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76
$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A
$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70
$GPGSV,3,2,11,02,39,223,19,13,28,070,17,26,23,252,,04,14,186,14*79
$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76
$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43
$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75
$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A
$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70
$GPGSV,3,2,11,02,39,223,16,13,28,070,17,26,23,252,,04,14,186,15*77
$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76
$GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A*45
```

## Contraintes

* Votre programme doit être écrit en C ou C++
* Si vous utilisez le C++, vous pouvez utiliser jusqu'à la version 20 du standard
* Votre programme doit être compilable sur Linux **et** sur Windows
* Vous devez fournir un moyen de compiler votre programme (**CMake** est recommandé)
* Vous avez le droit d'utiliser des bibliothèques externes, mais vous devez faire en sorte que la compilation de votre programme ne demande pas de manip reloue (`apt install` est toléré, mais c'est tout)
* Vous devez traiter cet exercice comme un vrai projet. Encore une fois, le but est d'évaluer votre capacité à concevoir un programme extensible, et non pas de parser des messages NMEA. Cette partie est secondaire (mais elle n'en reste pas moins **obligatoire** !)

## Rendu

Vous êtes libre de choisir la manière dont vous allez rendre votre projet, mais fournir un lien vers un dépôt Git est recommandé.

Aucune restriction n'est imposée quand à l'organisation de votre dépôt, mais il doit être facile de comprendre comment compiler et exécuter votre programme (par exemple, en fournissant un fichier `README.md`).

## Délai

Aucun délai n'est imposé. Vous êtes encouragés à prendre votre temps pour faire quelque chose de propre et de bien pensé.

Vraiment, prenez votre temps. C'est important.

## Questions

N'hésitez pas à poser des questions si vous en avez. Vous pouvez même demander de l'aide ou des conseils d'implémentation si vous en avez besoin. Vous ne serez **pas** pénalisés pour ça. Au contraire, ça montrera que vous êtes capables de demander de l'aide quand vous en avez besoin, et de challenger vos idées, et les nôtres.

Bien que le but soit de vous évaluer, ceci n'est pas un "examen". Nous voulons que vous abordiez cet exercice comme n'importe quel autre projet, et que vous vous comportiez comme vous le feriez dans un contexte professionnel. Vous avez **bien évidemment** le droit de regarder sur Internet, d'utiliser des outils, de nous solliciter, de demander de l'aide à quelqu'un d'autre (dans la limite du raisonnable, bien sûr).

Vous devez simplement être prêts à expliquer vos choix, et à justifier vos décisions. Nous sommes surtout intéressés par votre raisonnement et vos choix techniques.

