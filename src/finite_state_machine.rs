// finite_state_machine.rs
use std::collections::{HashMap, HashSet};

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum MachineStatus {
    Idle,
    Running,
    Success,
    Error,
}

pub struct FiniteStateMachine {
    transitions: HashMap<String, Box<dyn Fn(char, bool) -> String>>,
    initial_state: String,
    final_states: HashSet<String>,
    cursor_back_states: HashSet<String>,
    current_state: String,
    lexeme: String,
    status: MachineStatus,
}

impl FiniteStateMachine {
    pub fn new(
        transitions: HashMap<String, Box<dyn Fn(char, bool) -> String>>,
        initial_state: String,
        final_states: HashSet<String>,
        cursor_back_states: HashSet<String>,
    ) -> Self {
        FiniteStateMachine {
            transitions,
            initial_state: initial_state.clone(),
            final_states,
            cursor_back_states,
            current_state: initial_state,
            lexeme: String::new(),
            status: MachineStatus::Idle,
        }
    }

    pub fn status(&self) -> MachineStatus {
        self.status
    }

    pub fn lexeme(&self) -> &str {
        &self.lexeme
    }

    pub fn reset(&mut self) {
        self.current_state = self.initial_state.clone();
        self.lexeme.clear();
        self.status = MachineStatus::Idle;
    }

    pub fn transition(&mut self, c: char, is_eof: bool) -> MachineStatus {
        if self.status == MachineStatus::Error || self.status == MachineStatus::Success {
            return self.status;
        }

        let transition_fn = match self.transitions.get(&self.current_state) {
            Some(func) => func,
            None => {
                self.status = MachineStatus::Error;
                return self.status;
            }
        };

        let next_state = transition_fn(c, is_eof);
        self.current_state = next_state.clone();
        self.status = MachineStatus::Running;

        if next_state == "dead" {
            self.status = MachineStatus::Error;
            return self.status;
        }

        if self.final_states.contains(&next_state) {
            self.status = MachineStatus::Success;
        }

        if !self.should_rewind() {
            self.lexeme.push(c);
        }

        self.status
    }

    pub fn should_rewind(&self) -> bool {
        if self.status != MachineStatus::Success {
            return false;
        }
        if !self.final_states.contains(&self.current_state) {
            return false;
        }
        self.cursor_back_states.contains(&self.current_state)
    }
}