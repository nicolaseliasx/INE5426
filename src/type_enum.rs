// type_enum.rs
use std::fmt;
use std::string::ToString;
use crate::errors::SemanticError;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Type {
    Function,
    Int,
    Float,
    String,
}

impl Type {
    pub fn to_str(self) -> &'static str {
        match self {
            Type::Function => "FUNCTION",
            Type::Int => "INT",
            Type::Float => "FLOAT",
            Type::String => "STRING",
        }
    }
}

impl fmt::Display for Type {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.to_str())
    }
}

impl ToString for Type {
    fn to_string(&self) -> String {
        self.to_str().to_string()
    }
}

impl TryFrom<&str> for Type {
    type Error = SemanticError;
    
    fn try_from(value: &str) -> Result<Self, Self::Error> {
        match value {
            "FUNCTION" => Ok(Type::Function),
            "INT" => Ok(Type::Int),
            "FLOAT" => Ok(Type::Float),
            "STRING" => Ok(Type::String),
            _ => Err(SemanticError::new(format!("Invalid type string: {}", value))),
        }
    }
}

// Operador + para String + Type
impl std::ops::Add<Type> for String {
    type Output = String;
    
    fn add(self, rhs: Type) -> String {
        self + rhs.to_str()
    }
}

// Operador + para &str + Type
impl std::ops::Add<Type> for &str {
    type Output = String;
    
    fn add(self, rhs: Type) -> String {
        self.to_string() + rhs.to_str()
    }
}