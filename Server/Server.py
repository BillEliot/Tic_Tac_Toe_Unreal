from socket import *
import copy
import pymysql
import threading
import json

class Server(object):
    def __init__(self, ip, port):
        self.address = (ip, port)
        self.bufsiz = 2048
        '''
        key: socket
        value: {
            'addr': addr,
            'thread': t,
        }
        '''
        self.clients = {}
        '''
        key: room_id
        value: {
            'title': title,
            'master_uid': uid,
            'master_username': masterUsername,
            'teamA': [{
                'uid': uid,
                'username': username,
                'bMaster': True,
                'bReady': True,
                'socket': socket
            }],
            'teamB': [],
            'board': [][], 0-none, 1-correct, 2-wrong
            'current': uid
        }
        '''
        self.rooms = {}
 

    def start(self):
        print('[+] Connect to mysql')
        self.db = pymysql.connect("localhost", "root", "suxiaomo8299502.", "game_board")
        self.cursor = self.db.cursor()

        print('[+] Create Socket')
        self.tcpServerSock = socket(AF_INET,SOCK_STREAM)
        self.tcpServerSock.bind(self.address)
        self.tcpServerSock.listen(5)
        print('[+] Accepting')
        self.accept()

    def accept(self):
        t = threading.Thread(target=self.thread_accept)
        t.setDaemon(True)
        t.start()

    def thread_accept(self):
        while True:
            tcpClientSock, addr = self.tcpServerSock.accept()
            print ('Connected from :', addr)
            t = threading.Thread(target=self.thread_recv, args=(tcpClientSock,))
            self.clients[tcpClientSock] = {
                'addr': addr,
                'thread': t
            }
            t.setDaemon(True)
            t.start()
    
    def thread_recv(self, clientSocket):
        while True:
            data = clientSocket.recv(self.bufsiz)
            self.handleData(clientSocket, data.decode('utf-8', 'ignore'))

    def handleData(self, clientSocket, data):
        sign, info = data.split(':')
        if sign == 'register':
            username, password, email = info.split(',')
            self.cursor.execute("SELECT * FROM user WHERE username='%s'" % username)
            result = self.cursor.fetchone()
            if result:
                clientSocket.sendall(json.dumps({
                    'sign': 'register',
                    'code': 1,
                    'data': {}
                }).encode())
            else:
                self.cursor.execute("SELECT * FROM user WHERE email='%s'" % email)
                result = self.cursor.fetchone()
                if result:
                    clientSocket.sendall(json.dumps({
                        'sign': 'register',
                        'code': 2,
                        'data': {}
                    }).encode())
                else:
                    self.cursor.execute("INSERT INTO user (username, password, email, creation_date) VALUES ('%s', '%s', '%s', NOW())" % (username, password, email))
                    clientSocket.sendall(json.dumps({
                        'sign': 'register',
                        'code': 0,
                        'data': {}
                    }).encode())
        elif sign == 'login':
            username, password = info.split(',')
            self.cursor.execute("SELECT * FROM user WHERE username='%s'" % username)
            result = self.cursor.fetchone()
            if result:
                if result[2] == password:
                    # login successfully
                    clientSocket.sendall(json.dumps({
                        'sign': 'login',
                        'code': 0,
                        'data': {
                            'uid': result[0],
                            'username': result[1],
                            'email': result[3],
                            'rooms': self.rooms_json(self.rooms)
                        }
                    }).encode())
                else:
                    clientSocket.sendall(json.dumps({
                        'sign': 'login',
                        'code': 1,
                        'data': {}
                    }).encode())
            else:
                clientSocket.sendall(json.dumps({
                    'sign': 'login',
                    'code': 2,
                    'data': {}
                }).encode())
        elif sign == 'create_room':
            title, uid, username = info.split(',')
            uid = int(uid)
            self.rooms[len(self.rooms)] = {
                'title': title,
                'master_uid': uid,
                'master_username': username,
                'teamA': [],
                'teamB': [],
                'board': [[0,0,0], [0,0,0], [0,0,0]],
                'current': -1
            }
            # update new room to all clients
            for clientSocket in self.clients.keys():
                clientSocket.sendall(json.dumps({
                    'sign': 'create_room',
                    'code': 0,
                    'data': {
                        'room_id': len(self.rooms)-1,
                        'data': self.room_json(self.rooms[len(self.rooms)-1])
                    }
                }).encode())
        elif sign == 'join_room':
            room_id, uid, username = info.split(',')
            room_id = int(room_id)
            uid = int(uid)
            playerCount = len(self.rooms[room_id]['teamA']) + len(self.rooms[room_id]['teamB'])

            team = ''
            if len(self.rooms[room_id]['teamA']) > len(self.rooms[room_id]['teamB']):
                team = 'B'
                if playerCount == 0:
                    # master
                    self.rooms[room_id]['master_uid'] = uid
                    self.rooms[room_id]['master_username'] = username
                    self.rooms[room_id]['teamB'].append({
                        'uid': uid,
                        'username': username,
                        'bMaster': True,
                        'bReady': True,
                        'socket': clientSocket
                    })
                else:
                    self.rooms[room_id]['teamB'].append({
                        'uid': uid,
                        'username': username,
                        'bMaster': False,
                        'bReady': False,
                        'socket': clientSocket
                    })
            else:
                team = 'A'
                if playerCount == 0:
                    # master
                    self.rooms[room_id]['master_uid'] = uid
                    self.rooms[room_id]['master_username'] = username
                    self.rooms[room_id]['teamA'].append({
                        'uid': uid,
                        'username': username,
                        'bMaster': True,
                        'bReady': True,
                        'socket': clientSocket
                    })
                else:
                    self.rooms[room_id]['teamA'].append({
                        'uid': uid,
                        'username': username,
                        'bMaster': False,
                        'bReady': False,
                        'socket': clientSocket
                    })
            # update room info to the new player
            clientSocket.sendall(json.dumps({
                'sign': 'join_room',
                'code': 0,
                'data': {
                    'room_id': room_id,
                    'team': team,
                    'data': self.room_json(self.rooms[room_id])
                }
            }).encode())
            # update the new player' info to other players in the room
            for player in self.rooms[room_id]['teamA']:
                if player['socket'] != clientSocket:
                    player['socket'].sendall(json.dumps({
                        'sign': 'update_room_player',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'username': username,
                            'team': team
                        }
                    }).encode())
            for player in self.rooms[room_id]['teamB']:
                if player['socket'] != clientSocket:
                    player['socket'].sendall(json.dumps({
                        'sign': 'update_room_player',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'username': username,
                            'team': team
                        }
                    }).encode())
            # update room player count(+1) to all players
            for socket in self.clients.keys():
                socket.sendall(json.dumps({
                    'sign': 'update_room_player_count',
                    'code': 0,
                    'data': {
                        'room_id': room_id
                    }
                }).encode())
        elif sign == 'room_message':
            room_id, username, message = info.split(',')
            room_id = int(room_id)
            for player in self.rooms[room_id]['teamA']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_message',
                    'code': 0,
                    'data': {
                        'username': username,
                        'message': message
                    }
                }).encode())
            for player in self.rooms[room_id]['teamB']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_message',
                    'code': 0,
                    'data': {
                        'username': username,
                        'message': message
                    }
                }).encode())
        elif sign == 'room_ready':
            room_id, uid = info.split(',')
            room_id = int(room_id)
            uid = int(uid)
            ready = False
            # change ready state
            bFound = False
            for player in self.rooms[room_id]['teamA']:
                if player['uid'] == uid:
                    player['bReady'] = not player['bReady']
                    ready = player['bReady']
                    bFound = True
                    break
            if not bFound:
                for player in self.rooms[room_id]['teamB']:
                    if player['uid'] == uid:
                        player['bReady'] = not player['bReady']
                        ready = player['bReady']
                        break
            # update the player's ready state to others in the room
            for player in self.rooms[room_id]['teamA']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_ready',
                    'code': 0,
                    'data': {
                        'uid': uid,
                        'ready': ready
                    }
                }).encode())
            for player in self.rooms[room_id]['teamB']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_ready',
                    'code': 0,
                    'data': {
                        'uid': uid,
                        'ready': ready
                    }
                }).encode())
        elif sign == 'room_start':
            room_id, uid = info.split(',')
            room_id = int(room_id)
            uid = int(uid)
            # not master
            if uid != self.rooms[room_id]['master_uid']:
                clientSocket.sendall(json.dumps({
                    'sign': 'room_start',
                    'code': 1,
                    'data': {}
                }).encode())
                return
            # not everyone is ready
            for player in self.rooms[room_id]['teamA']:
                if not player['bReady']:
                    clientSocket.sendall(json.dumps({
                        'sign': 'room_start',
                        'code': 2,
                        'data': {}
                    }).encode())
                    return
            for player in self.rooms[room_id]['teamB']:
                if not player['bReady']:
                    clientSocket.sendall(json.dumps({
                        'sign': 'room_start',
                        'code': 2,
                        'data': {}
                    }).encode())
                    return
            # start
            self.rooms[room_id]['current'] = self.rooms[room_id]['master_uid']
            for player in self.rooms[room_id]['teamA']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_start',
                    'code': 0,
                    'data': {
                        'master_uid': self.rooms[room_id]['master_uid']
                    }
                }).encode())
            for player in self.rooms[room_id]['teamB']:
                player['socket'].sendall(json.dumps({
                    'sign': 'room_start',
                    'code': 0,
                    'data': {
                        'master_uid': self.rooms[room_id]['master_uid']
                    }
                }).encode())
        elif sign == 'game_play':
            uid, room_id, team, x, y = info.split(',')
            room_id = int(room_id)
            uid = int(uid)
            x = int(x)
            y = int(y)

            # if current player
            if self.rooms[room_id]['current'] != uid:
                clientSocket.sendall(json.dumps({
                    'sign': 'game_play',
                    'code': 2,
                    'data': {}
                }).encode())
                return
            # correct
            if team == 'A':
                self.rooms[room_id]['board'][x][y] = 1
                if self.board_check(self.rooms[room_id]['board'], 1):
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())
                    # restore ready state
                    self.rooms[room_id]['teamB'][0]['bReady'] = False
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'room_ready',
                        'code': 0,
                        'data': {
                            'uid': self.rooms[room_id]['teamB'][0]['uid'],
                            'ready': False
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'room_ready',
                        'code': 0,
                        'data': {
                            'uid': self.rooms[room_id]['teamB'][0]['uid'],
                            'ready': False
                        }
                    }).encode())
                else:
                    self.rooms[room_id]['current'] = self.rooms[room_id]['teamB'][0]['uid']
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 1,
                        'data': {
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamB'][0]['uid']
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 1,
                        'data': {
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamB'][0]['uid']
                        }
                    }).encode())
            # wrong
            elif team == 'B':
                self.rooms[room_id]['board'][x][y] = 2
                if self.board_check(self.rooms[room_id]['board'], 2):
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 0,
                        'data': {
                            'uid': uid,
                            'x': x,
                            'y': y,
                            'type': 'correct',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())
                    # restore ready state
                    self.rooms[room_id]['teamB'][0]['bReady'] = False
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'room_ready',
                        'code': 0,
                        'data': {
                            'uid': self.rooms[room_id]['teamB'][0]['uid'],
                            'ready': False
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'room_ready',
                        'code': 0,
                        'data': {
                            'uid': self.rooms[room_id]['teamB'][0]['uid'],
                            'ready': False
                        }
                    }).encode())
                else:
                    self.rooms[room_id]['current'] = self.rooms[room_id]['teamA'][0]['uid']
                    self.rooms[room_id]['teamA'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 1,
                        'data': {
                            'x': x,
                            'y': y,
                            'type': 'wrong',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())
                    self.rooms[room_id]['teamB'][0]['socket'].sendall(json.dumps({
                        'sign': 'game_play',
                        'code': 1,
                        'data': {
                            'x': x,
                            'y': y,
                            'type': 'wrong',
                            'current': self.rooms[room_id]['teamA'][0]['uid']
                        }
                    }).encode())



    def room_json(self, room):
        if 'title' not in room:
            return {}

        _room = {
            'title': '',
            'master_uid': -1,
            'master_username': '',
            'teamA': [],
            'teamB': [],
        }
        _room['title'] = room['title']
        _room['master_uid'] = room['master_uid']
        _room['master_username'] = room['master_username']
        for player in room['teamA']:
            _room['teamA'].append({
                'uid': player['uid'],
                'username': player['username'],
                'bMaster': player['bMaster'],
                'bReady': player['bReady']
            })
        for player in room['teamB']:
            _room['teamB'].append({
                'uid': player['uid'],
                'username': player['username'],
                'bMaster': player['bMaster'],
                'bReady': player['bReady']
            })
        return _room

    def rooms_json(self, rooms):
        _rooms = {}
        for (key, value) in rooms.items():
            _rooms[key] = self.room_json(value)

        return _rooms

    def board_check(self, board, ty):
        if board[0][0] == board[0][1] and board[0][1] == board[0][2] and board[0][0] == ty:
            return True
        if board[1][0] == board[1][1] and board[1][1] == board[1][2] and board[1][0] == ty:
            return True
        if board[2][0] == board[2][1] and board[2][1] == board[2][2] and board[2][0] == ty:
            return True

        if board[0][0] == board[1][0] and board[1][0] == board[2][0] and board[0][0] == ty:
            return True
        if board[0][1] == board[1][1] and board[1][1] == board[2][1] and board[0][1] == ty:
            return True
        if board[0][2] == board[1][2] and board[1][2] == board[2][2] and board[0][2] == ty:
            return True

        if board[0][0] == board[1][1] and board[1][1] == board[2][2] and board[0][0] == ty:
            return True
        if board[0][2] == board[1][1] and board[1][1] == board[2][0] and board[0][2] == ty:
            return True

        return False

    def exit(self):
        self.tcpServerSock.close()
        self.db.close()


if __name__ == "__main__":
    server = Server('127.0.0.1', 1081)
    server.start()
    while True:
        msg = input('input:')
        if msg == '0':
            server.exit()
