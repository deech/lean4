def foo (_ : Nat) := (10 : UInt32)

example : foo x = 8 + 2 := by
  simp
  trace_state
  rw [foo]

example : foo x = 5 * 2 := by
  simp
  trace_state
  rw [foo]

example : foo x = 12 - 2 := by
  simp
  trace_state
  rw [foo]

example : foo x = 20 / 2 := by
  simp
  trace_state
  rw [foo]

example : foo x = 110 % 100 := by
  simp
  trace_state
  rw [foo]

example : foo x = (3+2)*2 := by
  simp
  trace_state
  rw [foo]


def boo (_ : Nat) := True

example : boo x ↔ (2 : UInt32) < 3 := by
  simp
  trace_state
  trivial

example : boo x ↔ (3 : UInt32) > 2 := by
  simp
  trace_state
  trivial

example : boo x ↔ (2 : UInt32) ≤ 3 := by
  simp
  trace_state
  trivial

example : boo x ↔ (3 : UInt32) ≥ 2 := by
  simp
  trace_state
  trivial
