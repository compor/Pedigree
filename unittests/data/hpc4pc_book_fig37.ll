
define void @foo() #0 {
entry:
  br label %b_label

b_label:                                          ; preds = %entry
  %add = add nsw i32 0, 3
  br label %while.cond

while.cond:                                       ; preds = %while.end, %b_label
  %i.0 = phi i32 [ 0, %b_label ], [ %i.1, %while.end ]
  %k.0 = phi i32 [ %add, %b_label ], [ %inc7, %while.end ]
  %cmp = icmp slt i32 %k.0, 10
  br i1 %cmp, label %while.body, label %while.end.8

while.body:                                       ; preds = %while.cond
  br label %while.cond.1

while.cond.1:                                     ; preds = %if.end, %while.body
  %i.1 = phi i32 [ %i.0, %while.body ], [ %inc, %if.end ]
  %cmp2 = icmp slt i32 %i.1, 30
  br i1 %cmp2, label %while.body.3, label %while.end

while.body.3:                                     ; preds = %while.cond.1
  %cmp4 = icmp slt i32 %i.1, 5
  br i1 %cmp4, label %if.then, label %if.else

if.then:                                          ; preds = %while.body.3
  %add5 = add nsw i32 %i.1, 2
  br label %if.end

if.else:                                          ; preds = %while.body.3
  %add6 = add nsw i32 %i.1, 3
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %i.2 = phi i32 [ %add5, %if.then ], [ %add6, %if.else ]
  %inc = add nsw i32 %i.2, 1
  br label %while.cond.1

while.end:                                        ; preds = %while.cond.1
  %inc7 = add nsw i32 %k.0, 1
  br label %while.cond

while.end.8:                                      ; preds = %while.cond
  br label %i_label

i_label:                                          ; preds = %while.end.8
  %add9 = add nsw i32 %k.0, 7
  br label %j_label

j_label:                                          ; preds = %i_label
  %add10 = add nsw i32 %add9, 4
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

