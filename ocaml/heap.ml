(*Module Heap*)
(*Définition des types*)
type 'a couple = Element of 'a * int;;
type 'a tas = Vide | Noeud of 'a tas * 'a couple * 'a tas;;

(*Fonctions usuelles d'une structure de tas*)
let creerTas () = Vide;;

let rec ajout t (Element(elt,freq)) = match t with
|Vide -> Noeud(Vide,Element(elt,freq),Vide)
|Noeud(tg,Element(a,b),td) when freq < b -> Noeud(td,Element(elt,freq),ajout tg (Element(a,b)))
|Noeud(tg,Element(a,b),td) -> Noeud(td,Element(a,b),ajout tg (Element(elt,freq)));;

let rec ajoutListe t liste = match liste with
	|[] -> t
	|a::q -> ajoutListe (ajout t a) q;;

let ajoutListe2 liste = 
	let rec aux t l = match l with
	|[] -> t
	|a::q -> aux (ajout t a) q
	in aux Vide liste;;

let rec popRacine t = match t with
|Vide -> failwith"Le tas est vide"
|Noeud(Vide,_,Vide) -> Vide
|Noeud(tg,_,Vide) -> tg
|Noeud(Vide,_,td) -> td
|Noeud(Noeud(tgg,Element(eltg,freqg),tgd),_,Noeud(tdg,Element(eltd,freqd),tdd)) when freqg < freqd -> Noeud(popRacine (Noeud(tgg,Element(eltg,freqg),tgd)),Element(eltg,freqg),Noeud(tdg,Element(eltd,freqd),tdd))
|Noeud(Noeud(tgg,Element(eltg,freqg),tgd),_,Noeud(tdg,Element(eltd,freqd),tdd)) -> Noeud(Noeud(tgg,Element(eltg,freqg),tgd),Element(eltd,freqd),popRacine(Noeud(tdg,Element(eltd,freqd),tdd)));;

let popMinElement t = match t with
|Vide -> failwith "Le tas est vide"
|Noeud(_,a,_) -> (a, popRacine t);;
