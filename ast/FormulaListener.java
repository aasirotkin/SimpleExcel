// Generated from Formula.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link FormulaParser}.
 */
public interface FormulaListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link FormulaParser#main}.
	 * @param ctx the parse tree
	 */
	void enterMain(FormulaParser.MainContext ctx);
	/**
	 * Exit a parse tree produced by {@link FormulaParser#main}.
	 * @param ctx the parse tree
	 */
	void exitMain(FormulaParser.MainContext ctx);
	/**
	 * Enter a parse tree produced by the {@code UnaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterUnaryOp(FormulaParser.UnaryOpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code UnaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitUnaryOp(FormulaParser.UnaryOpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code Parens}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterParens(FormulaParser.ParensContext ctx);
	/**
	 * Exit a parse tree produced by the {@code Parens}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitParens(FormulaParser.ParensContext ctx);
	/**
	 * Enter a parse tree produced by the {@code Literal}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterLiteral(FormulaParser.LiteralContext ctx);
	/**
	 * Exit a parse tree produced by the {@code Literal}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitLiteral(FormulaParser.LiteralContext ctx);
	/**
	 * Enter a parse tree produced by the {@code BinaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterBinaryOp(FormulaParser.BinaryOpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code BinaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitBinaryOp(FormulaParser.BinaryOpContext ctx);
}