open Core.Std
open Sys

let daxpy z a x y =
  for i = 0 to Array.length x - 1 do
    Array.set z i (a *. Array.get x i +. Array.get y i)
  done

let time f =
  let t = Unix.gettimeofday () in
  let fx = f () in
  (Unix.gettimeofday () -. t) *. 1000.0


let bench n m f valx valy valz vala =
  let z = Array.create n valz in
  let a = vala in
  let x = Array.create n valx in
  let y = Array.create n valy in
  time (fun () -> for i = 1 to m do
                    f z a x y
                  done)

let () =
  let n = [10; 100; 1_000; 10_000; 100_000; 1_000_000; 10_000_000; 100_000_000] in
  let m = 10 in
  List.iter ~f:(fun i -> let runtime = bench i m daxpy 1.5 3.7 0.0 1.5 in
                         Printf.printf "%d, %f\n" i (runtime /. float_of_int m))
            n
