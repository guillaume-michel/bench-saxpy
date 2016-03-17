open Core.Std
open Sys

let saxpy z a x y =
  for i = 0 to Array.length x - 1 do
    Array.set z i (a *. Array.get x i +. Array.get y i)
  done

let time f =
  let t = Unix.gettimeofday () in
  let fx = f () in
  Printf.printf "execution elapsed time: %f sec\n"
                (Unix.gettimeofday () -. t);
  fx


let bench_saxpy () =
  let n = 100_000_000 in
  let z = Array.create n 0.0 in
  let a = 1.5 in
  let x = Array.create n 5.3 in
  let y = Array.create n 7.5 in
  time (fun () -> saxpy z a x y)

let () =
  bench_saxpy ()
