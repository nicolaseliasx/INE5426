// errors.rs
use std::fmt;
use std::error::Error;
use std::fmt::{Display, Formatter};
use crate::token::Token;

#[derive(Debug, Clone, PartialEq)]
pub struct ErrorLocation {
    pub line: usize,
    pub column: usize,
    pub file: Option<String>,
}

impl ErrorLocation {
    pub fn new(line: usize, column: usize) -> Self {
        Self { line, column, file: None }
    }
    
    pub fn with_file(line: usize, column: usize, file: &str) -> Self {
        Self { line, column, file: Some(file.to_string()) }
    }

    // Novo método: criar a partir de um token
    pub fn from_token(token: &Token) -> Self {
        Self::new(token.line, token.column)
    }
}

impl Display for ErrorLocation {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        if let Some(file) = &self.file {
            write!(f, "{}:{}:{}", file, self.line, self.column)
        } else {
            write!(f, "{}:{}", self.line, self.column)
        }
    }
}

#[derive(Debug)]
pub enum SemanticErrorType {
    UndeclaredSymbol,
    TypeMismatch,
    Redeclaration,
    InvalidOperation,
    ScopeViolation,
    ArgumentMismatch,
    Other(String),
}

#[derive(Debug)]
pub enum CompilationError {
    Lexical {
        message: String,
        location: ErrorLocation,
    },
    Syntax {
        message: String,
        location: ErrorLocation,
        expected: Vec<String>,
    },
    Semantic {
        error_type: SemanticErrorType,
        message: String,
        location: ErrorLocation,
        related: Option<ErrorLocation>,
    },
}

impl Display for CompilationError {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        match self {
            CompilationError::Lexical { message, location } => {
                write!(f, "Lexical error at {}: {}", location, message)
            }
            CompilationError::Syntax { message, location, expected } => {
                write!(f, "Syntax error at {}: {}", location, message)?;
                if !expected.is_empty() {
                    write!(f, "\nExpected one of: {}", expected.join(", "))?;
                }
                Ok(())
            }
            CompilationError::Semantic { error_type, message, location, related } => {
                let error_name = match error_type {
                    SemanticErrorType::UndeclaredSymbol => "Undeclared symbol",
                    SemanticErrorType::TypeMismatch => "Type mismatch",
                    SemanticErrorType::Redeclaration => "Redeclaration",
                    SemanticErrorType::InvalidOperation => "Invalid operation",
                    SemanticErrorType::ScopeViolation => "Scope violation",
                    SemanticErrorType::ArgumentMismatch => "Argument mismatch",
                    SemanticErrorType::Other(_) => "Semantic error",
                };
                
                write!(f, "{} at {}: {}", error_name, location, message)?;
                
                if let Some(rel) = related {
                    write!(f, "\nRelated to: {}", rel)?;
                }
                Ok(())
            }
        }
    }
}

impl Error for CompilationError {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        None
    }
}

impl CompilationError {
    // Construtores melhorados com localização
    pub fn lexical<S: Into<String>>(message: S, location: ErrorLocation) -> Self {
        CompilationError::Lexical {
            message: message.into(),
            location,
        }
    }
    
    // Construtor lexical simplificado
    pub fn lexical_simple<S: Into<String>>(message: S, line: usize, column: usize) -> Self {
        Self::lexical(message, ErrorLocation::new(line, column))
    }
    
    pub fn syntax<S: Into<String>>(
        message: S,
        location: ErrorLocation,
        expected: Vec<&str>
    ) -> Self {
        CompilationError::Syntax {
            message: message.into(),
            location,
            expected: expected.iter().map(|s| s.to_string()).collect(),
        }
    }
    
    // Construtor syntax simplificado
    pub fn syntax_simple<S: Into<String>>(
        message: S,
        line: usize,
        column: usize,
        expected: Vec<&str>
    ) -> Self {
        Self::syntax(message, ErrorLocation::new(line, column), expected)
    }
    
    pub fn semantic<S: Into<String>>(
        error_type: SemanticErrorType,
        message: S,
        location: ErrorLocation,
    ) -> Self {
        CompilationError::Semantic {
            error_type,
            message: message.into(),
            location,
            related: None,
        }
    }
    
    // Construtor semantic simplificado
    pub fn semantic_simple<S: Into<String>>(
        error_type: SemanticErrorType,
        message: S,
        line: usize,
        column: usize,
    ) -> Self {
        Self::semantic(error_type, message, ErrorLocation::new(line, column))
    }
    
    pub fn semantic_with_related<S: Into<String>>(
        error_type: SemanticErrorType,
        message: S,
        location: ErrorLocation,
        related: ErrorLocation,
    ) -> Self {
        CompilationError::Semantic {
            error_type,
            message: message.into(),
            location,
            related: Some(related),
        }
    }
    
    // Métodos auxiliares
    pub fn location(&self) -> &ErrorLocation {
        match self {
            CompilationError::Lexical { location, .. } => location,
            CompilationError::Syntax { location, .. } => location,
            CompilationError::Semantic { location, .. } => location,
        }
    }
    
    pub fn is_fatal(&self) -> bool {
    match self {
        CompilationError::Semantic {
            error_type: SemanticErrorType::Other(_),
            message: _,
            location: _,
            related: _,
        } => false,
        _ => true,
    }
}
}

// Agregador de erros para relatório completo
#[derive(Debug, Default)]
pub struct ErrorReporter {
    errors: Vec<CompilationError>,
    warnings: Vec<CompilationError>,
}

impl ErrorReporter {
    pub fn new() -> Self {
        Self {
            errors: Vec::new(),
            warnings: Vec::new(),
        }
    }
    
    pub fn add_error(&mut self, error: CompilationError) {
        self.errors.push(error);
    }
    
    pub fn add_warning(&mut self, warning: CompilationError) {
        self.warnings.push(warning);
    }
    
    pub fn has_errors(&self) -> bool {
        !self.errors.is_empty()
    }
    
    pub fn has_fatal_errors(&self) -> bool {
        self.errors.iter().any(|e| e.is_fatal())
    }
    
    pub fn print_report(&self, out: &mut impl fmt::Write) -> fmt::Result {
        for (i, error) in self.errors.iter().enumerate() {
            writeln!(out, "Error {}: {}", i + 1, error)?;
        }
        
        for (i, warning) in self.warnings.iter().enumerate() {
            writeln!(out, "Warning {}: {}", i + 1, warning)?;
        }
        
        writeln!(out, "\nTotal: {} errors, {} warnings", self.errors.len(), self.warnings.len())
    }
    
    pub fn into_errors(self) -> Vec<CompilationError> {
        self.errors
    }
}