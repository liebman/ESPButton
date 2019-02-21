#!/usr/bin/env python2.7

import os
from flask import Flask, redirect, current_app

app = Flask(__name__)

@app.route('/redirect/<int:count>')
def do_redirect(count):
    count -= 1
    if count < 0:
        return "Redirects Complete!\n"
    
    response = current_app.response_class(
        response="",
        status=301,
        mimetype="text/plain")
    response.headers["Location"] = "/redirect/{}".format(count)
    return response

@app.route('/')
def hello():
    return 'Hello World!'

if __name__ == '__main__':
    # Bind to PORT if defined, otherwise default to 5000.
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
