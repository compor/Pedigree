
; Function Attrs: nounwind uwtable
define i32 @foo(i32 %n) #0 {
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %i.0 = phi i32 [ 2, %if.then ], [ 0, %entry ]
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %j.0 = phi i32 [ undef, %if.end ], [ %add, %while.body ]
  %n.addr.0 = phi i32 [ %n, %if.end ], [ %call, %while.body ]
  %cmp1 = icmp sgt i32 %n.addr.0, 0
  br i1 %cmp1, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %add = add nsw i32 %i.0, 1
  %call = call i32 @goo(i32 %n.addr.0, i32 %i.0)
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret i32 %j.0
}

declare i32 @goo(i32, i32) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

