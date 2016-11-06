from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import gettempdir

from helpers import *
import json	
import re
# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = gettempdir()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    
    # get info about user
    infouser = db.execute('SELECT symbol, sum(shares) AS shares FROM userstock WHERE id=:id AND symbol is not null GROUP BY symbol ORDER BY symbol', \
        id=session["user_id"])
    cash = db.execute('SELECT cash FROM users WHERE id=:id', id=session["user_id"])
    # if user has no stocks open clear board
    if len(infouser) == 0:
        return render_template("index.html", arg1=infouser, arg5=usd(cash[0]['cash']), arg4=usd(cash[0]['cash']))
    
    # prepare temporary storages for saving prices, counting current total price 
    length = len(infouser) # take the number of different stocks 
    infostock = [] * length # for saving current price
    checkline = db.execute('SELECT * FROM total WHERE id=:id', id=session["user_id"])
    if len(checkline) > 0:
        db.execute('DELETE FROM total WHERE id=:id', id=session["user_id"])
    
    # get info about stocks and count current total price
    for i in range(length): 
        infostock.append(lookup(infouser[i]['symbol']))
        temptotal = infouser[i]['shares'] * infostock[i]['price']
        db.execute('INSERT INTO total (id, symbol, totalprice) VALUES (:id, :symbol, :totalprice)', \
            id=session["user_id"], symbol=infouser[i]['symbol'], totalprice=temptotal )
    total = db.execute('SELECT * FROM total WHERE id=:id', id=session["user_id"])
    alltotal = 0
    
    # format prices to usd
    for i in range(length):
        alltotal += (float)(total[i]['totalprice'])
        infostock[i]['price'] = usd(infostock[i]['price'])
        total[i]['totalprice'] = usd(total[i]['totalprice'])
    alltotal += cash[0]['cash']
    alltotal = usd(alltotal)
    
    return render_template("index.html", arg1=infouser, arg2=infostock, arg3=total, arg4 = alltotal, arg5 = usd(cash[0]['cash']))
    
@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol")

        # ensure number of shares was submitted
        elif not request.form.get("shares"):
            return apology("must provide shares")
        
        # ensure number of shares is positive and an int   
        number = request.form.get("shares")
        if re.search(r"\D", number)!=None:
            return apology("must provide positive number")
        if (int)(number) <= 0:
            return apology("must provide positive number")
        
        # saved user's choise
        stock = request.form.get("symbol")
        
        # request for stock's price and existance 
        result = lookup(stock)
        if result == None:
            return apology("invalid symbol")

        # query database for cash and count total amount of money for transaction
        cash = db.execute('SELECT cash FROM users WHERE id=:id', id=session["user_id"])
        cash1= cash[0]["cash"]
        total =  (int)(number) * (int)(result['price'])        
        
        # ensure user has enough cash for transaction
        if total > cash1:
            return apology("you haven't enough cash")
        
        # add row to the purchase table and update users table
        db.execute('INSERT INTO purchase (id, symbol, company, shares, price, buysell) \
                    VALUES (:id, :symbol, :name, :num, :price, :buysell)', \
                    id=session["user_id"], symbol=result["symbol"], name=result["name"], num=number, price=result["price"], buysell=1)
        db.execute('UPDATE users SET cash=:upcash WHERE id=:id', upcash=cash1-total, id=session["user_id"])
        
        # check if there is a data about current user in userstock
        checkuser = db.execute('SELECT * FROM userstock WHERE id=:id', id=session["user_id"])
        # if not, insert full info about user and transaction
        if len(checkuser) == 0:
            db.execute('INSERT INTO userstock (id, symbol, shares) VALUES (:id, :symbol, :shares)', id=session["user_id"], symbol=result["symbol"], shares=number)
        # check if there is a data about transacted symbol and insert/update info
        else:
            sharesbefore = db.execute('SELECT shares FROM userstock WHERE id=:id AND symbol=:symbol', id=session["user_id"], symbol=result["symbol"])
            if len(sharesbefore) == 0:
                db.execute('INSERT INTO userstock (id, symbol, shares) VALUES (:id, :symbol, :shares)', id=session["user_id"], symbol=result["symbol"], shares=number)
            else:
                db.execute('UPDATE userstock SET shares=:shares WHERE id=:id AND symbol=:symbol', \
                    shares=sharesbefore[0]['shares'] + (int)(number), id=session["user_id"], symbol=result["symbol"])
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")
    
@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    # get info from history table
    history = db.execute('SELECT symbol, shares, price, date, buysell FROM purchase WHERE id=:id ORDER BY symbol', id=session["user_id"])
    if history[0]['symbol'] == None:
        return render_template("history.html", arg1=history)
    length = len(history) # take the number of transactions 
    
    # format prices to usd
    for i in range(length):
        history[i]['price'] = usd(history[i]['price'])
    return render_template("history.html", arg1=history)
    
@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # if user reached route via GET (as by clicking a link)
    if request.method == "GET":
        return render_template("quote.html")
    
    # else if user reached route via POST (as by submitting a form via POST)
    else:
        
        # ensure stock's name was submitted in a right way
        if not request.form.get("typefield") and request.form.get("selectfield") == "1":
            return apology("Must select stock")
        
        # saved user's choise
        stock = request.form.get("typefield")
        
        # request for stock's price 
        result = lookup(stock)
        if result == None:
            return apology("invalid symbol")
        
        # redirect user to quoted page
        return render_template("quoted.html", name=result['name'], symbol=result['symbol'], price=result['price'])

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    
    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")
        
        # ensure password confirming was submitted
        elif not request.form.get("confirmPassword"):
            return apology("must retype password")
        
        # ensure username is unique
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        if len(rows) == 1:
            return apology("name was already taken") 
        
        # ensure password equals confirming
        if request.form.get("password") != request.form.get("confirmPassword"):
            return apology("must retype password")
        
        # add user to db 
        db.execute("INSERT INTO users (username, hash) VALUES (:name, :password)", \
                name=request.form.get("username"), password=pwd_context.encrypt(request.form.get("password")))
        
        # remember which user has logged in
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        session["user_id"] = rows[0]["id"]
        
        # redirect user to index page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")
        
@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol")

        # ensure number of shares was submitted
        elif not request.form.get("shares"):
            return apology("must provide shares")
        
        # ensure number of shares is positive and an int   
        number = request.form.get("shares")
        if re.search(r"\D", number)!=None:
            return apology("must provide positive number")
        if (int)(number) <= 0:
            return apology("must provide positive number")
        
        # saved user's choise
        stock = request.form.get("symbol").upper()
        
        # ensure user has enough stock for selling and count cash
        amount = db.execute('SELECT shares FROM userstock WHERE id=:id AND symbol=:symbol', id=session["user_id"], symbol=stock)
        if len(amount) == 0:
            return apology("you haven't such shares")
        if amount[0]['shares'] == None:
            return apology("you haven't enough shares")
        amount1= amount[0]["shares"]
        cash = db.execute('SELECT cash FROM users WHERE id= :id', id=session["user_id"])
        cash1= cash[0]["cash"]
        if (int)(number) > amount1:
            return apology("you haven't enough shares")
            
        # request for stock's price and existance 
        result = lookup(stock)
        if result == None:
            return apology("invalid symbol")

        # count total amount of income
        total =  (int)(number) * (int)(result['price'])        
        
        # add row to the purchase table and update users and userstock table
        db.execute('INSERT INTO purchase (id, symbol, company, shares, price, buysell) \
                    VALUES (:id, :symbol, :name, :num, :price, :buysell)', \
                    id=session["user_id"], symbol=result["symbol"], name=result["name"], num=number, price=result["price"], buysell=0)
        db.execute('UPDATE users SET cash=:upcash WHERE id=:id', upcash=cash1+total, id=session["user_id"])
        
        sharesbefore = db.execute('SELECT shares FROM userstock WHERE id= :id AND symbol=:symbol', id=session["user_id"], symbol=stock)
        shares = sharesbefore[0]['shares'] - (int)(number)
        if shares != 0:
            db.execute('UPDATE userstock SET shares=:shares WHERE id=:id AND symbol=:symbol', shares=shares, id=session["user_id"], symbol=stock)
        else:
            db.execute('DELETE FROM userstock WHERE id=:id AND symbol=:symbol', id=session["user_id"], symbol=stock)
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("sell.html")
        
@app.route("/balance", methods=["GET", "POST"])
@login_required
def balance():
    
    if request.method == "POST":
        
        cash = db.execute('SELECT cash FROM users WHERE id= :id', id=session["user_id"])
        cash1= cash[0]["cash"]
        
        # ensure amount was submitted in a right way
        if not request.form.get("add") and not request.form.get("get"):
            return apology("must define cash")
        if request.form.get("add") and request.form.get("get"):
            return apology("must set only one field")
    
        # update database
        if request.form.get("add"):
            db.execute('UPDATE users SET cash=:upcash WHERE id=:id', upcash=cash1+(int)(request.form.get("add")), id=session["user_id"])
        else:
            if cash1 < (int)(request.form.get("get")):
                return apology("you haven't enough cash")
            else:
                db.execute('UPDATE users SET cash=:upcash WHERE id=:id', upcash=cash1-(int)(request.form.get("get")), id=session["user_id"])
        return redirect(url_for("index"))
    
    if request.method == "GET":
        cash = db.execute('SELECT cash FROM users WHERE id= :id', id=session["user_id"])
        return render_template("balance.html", cash = usd(cash[0]['cash']))
    