exception EmptyFile;;
exception IOError;;

type input_buffer = {
  mutable file_end : bool;
  input_value : in_channel;
  mutable value : int;
  mutable bits_number : int;
  mutable next_value : string;
  mutable after_next_value : string;
};;

let create_input_buffer input_value =
  try
    let next_value = input_char input_value in
    let after_next_value = input_char input_value in
    {
      file_end = false;
      input_value;
      value = 0;
      bits_number = 0;
      next_value = String.make 1 next_value;
      after_next_value = String.make 1 after_next_value;
    }
  with Sys_error _ -> raise IOError;;

let input_char2 chann = 
  try 
    String.make 1 (input_char chann);
  with | End_of_file -> "";;

let read_bit input_buf=
  try
    if input_buf.bits_number <= 0 then begin
      if input_buf.file_end then
        raise IOError;
      input_buf.value <- Char.code input_buf.next_value.[0];
      input_buf.next_value <- input_buf.after_next_value;
      input_buf.bits_number <- 8;        
      input_buf.after_next_value <- input_char2 input_buf.input_value;
      if input_buf.after_next_value = "" then begin
        input_buf.file_end <- true;
        input_buf.bits_number <- Char.code input_buf.next_value.[0];   
      end
    end;
    let result = input_buf.value lsr 7 in
    input_buf.value <- (input_buf.value land 127) lsl 1;
    input_buf.bits_number <- input_buf.bits_number - 1;
    result
  with
  | IOError -> raise IOError;;

let read_byte (input_buf : input_buffer) =
  try
    let rec aux i next_byte =
      if i = 8 then
        next_byte
      else            
        let next_bit = read_bit input_buf in
        let updated_byte = (next_bit lsl (7 - i)) lor next_byte in
        aux (i + 1) updated_byte
    in
    aux 0 0
  with
  | IOError -> raise IOError;;
    
(*Output*)

type output_buffer = {
  output_value: out_channel;
  mutable valeur: int;
  mutable bits_number_out: int;
};;

let create_output_buffer output_value =
  {
    output_value;
    valeur = 0;
    bits_number_out = 0;
  };;

let reset_output_buffer buffer =
  buffer.valeur <- 0;
  buffer.bits_number_out <- 0;;

let write_bit buffer bit =
  try
    buffer.valeur <- (buffer.valeur lsl 1) lor bit;
    buffer.bits_number_out <- buffer.bits_number_out + 1;
    if buffer.bits_number_out >= 8 then begin
      output_byte buffer.output_value buffer.valeur;
      reset_output_buffer buffer
    end
  with
  | Sys_error msg -> raise (Sys_error msg)
  | _ -> raise (Sys_error "Output error");;

let write_byte buffer next_byte =
  try
    let tmp = ref next_byte in
    for i = 0 to 7 do
      let next_bit = !tmp lsr 7 in
      tmp := (!tmp lsl 1) land 255;
      write_bit buffer next_bit
    done
  with
  | Sys_error msg -> raise (Sys_error msg)
  | _ -> raise (Sys_error "Output error");;

let finish_output_buffer buffer =
  try
    if buffer.bits_number_out > 0 then begin
      buffer.valeur <- (buffer.valeur lsl (8 - buffer.bits_number_out));
    output_byte buffer.output_value buffer.valeur end;
    if buffer.bits_number_out = 0 then begin
      buffer.bits_number_out <- 8 end;
    output_byte buffer.output_value buffer.bits_number_out
  with
  | Sys_error msg -> raise (Sys_error msg)
  | _ -> raise (Sys_error "Output error");;