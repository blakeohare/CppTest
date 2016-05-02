#include <vector>

#include "nodes.h"
#include "../tokens/token.h"

using namespace std;
using namespace Nodes;

void Nodes::Assignment::SetLocalIdPass() { }
void Nodes::BinaryOpChain::SetLocalIdPass() { }
void Nodes::BooleanConstant::SetLocalIdPass() { }
void Nodes::BracketIndex::SetLocalIdPass() { }
void Nodes::BracketSlice::SetLocalIdPass() { }
void Nodes::ConstructorDefinition::SetLocalIdPass() { }
void Nodes::Dictionary::SetLocalIdPass() { }
void Nodes::DotField::SetLocalIdPass() { }
void Nodes::ExpressionAsExecutable::SetLocalIdPass() { }
void Nodes::FieldDefinition::SetLocalIdPass() { }
void Nodes::ForLoop::SetLocalIdPass() { }
void Nodes::FunctionDefinition::SetLocalIdPass() { }
void Nodes::FunctionInvocation::SetLocalIdPass() { }
void Nodes::InlineIncrement::SetLocalIdPass() { }
void Nodes::IntegerConstant::SetLocalIdPass() { }
void Nodes::IfStatement::SetLocalIdPass() { }
void Nodes::NullConstant::SetLocalIdPass() { }
void Nodes::StringConstant::SetLocalIdPass() { }
void Nodes::Ternary::SetLocalIdPass() { }
void Nodes::Variable::SetLocalIdPass() { }
