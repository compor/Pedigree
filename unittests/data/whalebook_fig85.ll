
; Function Attrs: nounwind uwtable
define void @foo(i32 %z) #0 {
entry:
  %cmp = icmp sgt i32 %z, 1
  br i1 %cmp, label %if.then, label %if.end.3

if.then:                                          ; preds = %entry
  %cmp1 = icmp sgt i32 %z, 0
  br i1 %cmp1, label %if.then.2, label %if.end.3

if.then.2:                                        ; preds = %if.then
  %add = add nsw i32 1, 1
  br label %return

if.end.3:                                         ; preds = %entry, %if.then
  %x.0 = phi i32 [ 1, %if.then ], [ 2, %entry ]
  %sub = sub nsw i32 %x.0, 3
  br label %return

return:                                           ; preds = %if.end.3, %if.then.2
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

