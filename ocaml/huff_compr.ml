(*Programme principal*)
type 'a arbreCompression = Leaf of 'a | Node of 'a arbreCompression * 'a arbreCompression;;

let creationArbre elt1 elt2 = Heap.Element(Leaf(elt2),elt1);;

let fusion (Heap.Element(arb1,a)) (Heap.Element(arb2,b)) = Heap.Element(Node(arb1,arb2),a+b);;

let countFrequence input =
	let res = Array.make 256 0 in
	try
	while true do
	let a = input_char input in
	res.(int_of_char a) <- res.(int_of_char a) +1  done; res
	with  | End_of_file -> res;;

let huffman_tree_huffman frequences = 	
	let rec aux i compteur liste = match i with
	|256 -> List.rev liste,compteur
	|_ when frequences.(i) <> 0 -> aux (i+1) (compteur+1) ((creationArbre frequences.(i) i)::liste)
	|_ -> aux (i+1) compteur liste in
	let listeArbres,taille = aux 0 0 [] in
	let arbre = ref (Heap.ajoutListe2 listeArbres) in
	for i=0 to taille-2 do
		let min1 = Heap.popMinElement !arbre in
		arbre := snd min1;
		let min2 = Heap.popMinElement !arbre in
		arbre := snd min2;
		arbre := Heap.ajout !arbre (fusion (fst min1) (fst min2)) done;
	fst (Heap.popMinElement !arbre);;

let code_compression arbreHuff =
	let code_table = Array.make 256 [] in
	let rec aux tmp arb = match arb with
	|Heap.Element(Leaf(a),b) -> code_table.(a) <- tmp;
	|Heap.Element(Node(fg,fd),a) -> aux (tmp @ [0]) (Heap.Element(fg,a));aux (tmp @ [1]) (Heap.Element(fd,a)) in
	aux [] arbreHuff;
	code_table;;
	
let rec huffman_tree_to_compress_file output_buffer huffman_tree = match huffman_tree with
|Heap.Element(Leaf(a),b) -> Buffers.write_bit output_buffer 0; Buffers.write_byte output_buffer a;
|Heap.Element(Node(fg,fd),a) -> Buffers.write_bit output_buffer 1; huffman_tree_to_compress_file output_buffer (Heap.Element(fg,a)); huffman_tree_to_compress_file output_buffer (Element(fd,a));;

let compression input_stream output_buffer code_table = 
try
	while true do
	let a = input_char input_stream in
	List.iter (Buffers.write_bit output_buffer) (code_table.(int_of_char a))  done;
	with  | End_of_file -> Buffers.finish_output_buffer output_buffer;;

let file_compression_wrapper entree sortie = 
let input_stream = open_in entree in
let output_value = open_out_bin sortie in
let output_buffer = Buffers.create_output_buffer output_value in
let tab = countFrequence input_stream in
let huffman_tree = huffman_tree_huffman tab in
let tabcode = code_compression huffman_tree in
seek_in input_stream 0;
huffman_tree_to_compress_file output_buffer huffman_tree;
compression input_stream output_buffer tabcode;
close_in input_stream;
close_out output_value;;

(*Décompression*)

let building_back_huffman_tree input_buffer =
	let rec aux () = 	
	let cc = Buffers.read_bit input_buffer in
	if cc=0 then let cc2 = Buffers.read_byte input_buffer in (creationArbre 0 cc2)
	else
	let left = aux () in
	let right = aux () in
	fusion left right in
	aux ();;

let decompression input_buffer output_buffer huffman_tree b=
  let nb_bits = ref 10 in
  let node = ref huffman_tree in
  let current_bit = ref 0 in
  try
    while !nb_bits > 0 do 
      match !node with 
      | Heap.Element (Leaf a, b) -> Buffers.write_byte output_buffer a; node := huffman_tree
      | Heap.Element (Node (fg, fd), a) -> 
        if input_buffer.Buffers.file_end && !nb_bits = 10 then begin
          current_bit := Buffers.read_bit input_buffer;
          nb_bits := input_buffer.bits_number + 1;
        end else begin
          current_bit := Buffers.read_bit input_buffer;
          if input_buffer.file_end && !nb_bits = 10 then
            nb_bits := input_buffer.bits_number + 1;
        end;
        if !current_bit = 0 then node := Heap.Element(fg, a)
        else node := Heap.Element(fd, a);
        if !nb_bits < 10 then decr nb_bits;
    done;

    match !node with
    | Heap.Element (Leaf a, b) -> Buffers.write_byte output_buffer a
    |_ -> ();
  with End_of_file -> ();;


let file_decompression_wrapper input_file_name output_file_name =
let input_stream = open_in_bin input_file_name in
let output_value = open_out_bin output_file_name in
let input_buffer = Buffers.create_input_buffer input_stream in
let output_buffer = Buffers.create_output_buffer output_value in
let huffman_tree = building_back_huffman_tree input_buffer in
decompression input_buffer output_buffer huffman_tree output_value;
close_in input_stream;
close_out output_value;;

file_compression_wrapper "./entree.txt" "./sortie";;
file_decompression_wrapper "./sortie" "./decompression.txt";;