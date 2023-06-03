# Le gateaupilateur

## Présentation

Il arrive à tout le monde d'avoir un fichier `.recette` qui a l'air appétissant, mais aucun moyen de le convertir en un fichier `.gateau` qui pourrait être mangé. C'est là que le gateaupilateur intervient. Il prend en entrée un fichier `.recette` et produit un fichier `.gateau` qui peut être mangé. C'est un peu comme un compilateur, mais pour les recettes de gateau.

## Contraintes

* Vous devez utiliser le C++.
* En aucun cas vous ne devez utiliser de bibliothèque externe (autre que la STL, bien sûr).
* En aucun cas votre programme ne doit planter (enfin, dans la limite du raisonnable).

## Format d'entrée

Le format d'entrée est un fichier texte. Il est constitué de plusieurs lignes. Chaque ligne est soit une ligne de commentaire, soit une ligne de recette. Les commentaires commencent par un # (pour cet exercice, les commentaires ne peuvent pas être sur plusieurs lignes, ni en fin de ligne). Les lignes de recette sont de la forme:

```txt
    ingredient quantite [unite]
```

* `ingredient` est une chaîne de caractères alphanumérique (les caractères spéciaux sont autorisés, mais pas les espaces).
* quantite est un nombre entier
* unité est un champ facultatif qui peut être:

| unité | signification    | bytecode |
|-------|------------------|----------|
| N/A   | unité indéfinie  | 0x00     |
| g     | gramme           | 0x01     |
| ml    | millilitre       | 0x02     |
| cs    | cuillère à soupe | 0x03     |
| cc    | cuillère à café  | 0x04     |

### Mauvais ingrédients

Il existe des ingrédients qui ne sont pas autorisés dans les recettes. Si un ingrédient est interdit, le gateaupilateur doit afficher un message d'erreur et quitter avec un code d'erreur non nul. Les ingrédients interdits sont:

* `dynamite`
* `poison`
* `poudre_de_perlimpinpin`

### Exemple valide

```txt
# Gateau au chocolat
chocolat 200 g
sucre 100 g
oeuf 2
farine 100 g
beurre 100 g
levure 1 cs
# J'adore la vanille, alors je mets un peu de vanille
vanille 1 cc
```

## Format de sortie

Le format de sortie est un fichier binaire. Il est composé d'un en-tête de 32 octets et d'un corps, sous le format suivant:

### En-tête

| offset | taille | description |
|--------|--------|-------------|
| 0   | 4      | Magic number: `CAKE` |
| 4   | 4      | Taille du corps (en octets) |
| 8   | 4      | Nombre d'ingrédients |
| 12  | 20     | Padding (`0x2B`) |

### Corps

Le corps est composé de plusieurs entrées. Chaque entrée est de la forme:

| offset | taille | description |
|--------|--------|-------------|
| 0      | 20     | Nom de l'ingrédient (padding avec des 0x00, 20 caractères max) |
| 20     | 4      | Quantité |
| 24     | 1      | Unité (voir la table plus haut) |
| 25     | 3      | Padding (`0x2B`) |

## Usage

```bash
gateaupilateur FICHIER [-o FICHIER_SORTIE]
```

* `FICHIER` est soit un fichier `.recette` soit un fichier `.gateau`. Si c'est un fichier `.recette`, le programme doit produire un fichier `.gateau`. Si c'est un fichier `.gateau`, le programme doit produire un fichier `.recette`.
* `-o FICHIER_SORTIE` est facultatif. Si il est présent, le fichier de sortie sera `FICHIER_SORTIE`. Si il n'est pas présent, le programme doit produire un fichier de sortie avec le même nom que le fichier d'entrée, mais avec l'extension `.gateau` ou `.recette` selon le cas.