PRAGMA user_version = 0;
PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;
PRAGMA synchronous  = NORMAL;

CREATE TABLE devices (
    id          INTEGER     NOT NULL PRIMARY KEY AUTOINCREMENT,
    name        TEXT        NOT NULL,
    pos_x       REAL        NOT NULL DEFAULT 0.0,
    pos_y       REAL        NOT NULL DEFAULT 0.0,
    comment     TEXT,

    CONSTRAINT name_not_empty CHECK (length(trim(name)) > 0),
    UNIQUE (name)
);

CREATE TABLE buses (
    id               INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    name             TEXT    NOT NULL,
    nominal_bit_rate INTEGER NOT NULL DEFAULT 500000,
    data_bit_rate    INTEGER NOT NULL DEFAULT 2000000,
    is_fd            INTEGER NOT NULL DEFAULT 0,
    color            INTEGER NOT NULL DEFAULT 0xFFFFFF,
    comment          TEXT,

    CONSTRAINT name_not_empty CHECK (length(trim(name)) > 0),
    CONSTRAINT is_fd_valid CHECK (is_fd IN (0,1)),

    UNIQUE (name)
);

CREATE TABLE bus_connections (
    device_id   INTEGER NOT NULL REFERENCES devices(id) ON DELETE CASCADE,
    bus_id      INTEGER NOT NULL REFERENCES buses(id)   ON DELETE CASCADE,

    PRIMARY KEY (device_id, bus_id)
);

CREATE TABLE messages (
    id              INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    bus_id          INTEGER NOT NULL,
    transmitter_id  INTEGER,
    can_id          INTEGER NOT NULL,
    is_extended     INTEGER NOT NULL DEFAULT 0,
    is_fd           INTEGER NOT NULL DEFAULT 0,
    dlc             INTEGER NOT NULL,
    name            TEXT    NOT NULL,
    cycle_time_ms   INTEGER          DEFAULT 0,
    comment         TEXT,

    FOREIGN KEY (bus_id) REFERENCES buses(id) ON DELETE CASCADE,
    FOREIGN KEY (transmitter_id) REFERENCES devices(id) ON DELETE SET NULL,

    CONSTRAINT is_extended_valid CHECK (is_extended IN (0,1)),
    CONSTRAINT is_fd_valid CHECK (is_fd IN (0,1)),
    CONSTRAINT dlc_valid CHECK (
        (dlc BETWEEN 0 AND 8) OR
        (is_fd = 1 AND dlc IN (12, 16, 20, 24, 32, 48, 64))
    ),
    CONSTRAINT cycle_time_ms_valid CHECK (cycle_time_ms >= 0),
    CONSTRAINT name_not_empty CHECK (length(trim(name)) > 0),
    CONSTRAINT valid_id_range CHECK (
        (is_extended = 0 AND can_id BETWEEN 0 AND 2047) OR
        (is_extended = 1 AND can_id BETWEEN 0 AND 536870911)
    ),

    UNIQUE (bus_id, can_id, is_extended)
);

CREATE TABLE signals (
    id          INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    message_id  INTEGER NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
    name        TEXT    NOT NULL,
    start_bit   INTEGER NOT NULL,
    bit_length  INTEGER NOT NULL,
    byte_order  INTEGER NOT NULL DEFAULT 0,
    is_signed   INTEGER NOT NULL DEFAULT 0,
    factor      REAL    NOT NULL DEFAULT 1.0,
    offset      REAL    NOT NULL DEFAULT 0.0,
    min_value   REAL    NOT NULL DEFAULT 0.0,
    max_value   REAL    NOT NULL DEFAULT 0.0,
    unit        TEXT    NOT NULL DEFAULT '',
    mux_type    INTEGER NOT NULL DEFAULT 0,
    mux_value   INTEGER          DEFAULT NULL,
    comment     TEXT,

    CONSTRAINT name_not_empty   CHECK (length(trim(name)) > 0),
    CONSTRAINT start_bit_valid  CHECK (start_bit >= 0 AND start_bit < 512),
    CONSTRAINT bit_length_valid CHECK (bit_length > 0 AND bit_length <= 512),
    CONSTRAINT byte_order_valid CHECK (byte_order IN (0,1)),
    CONSTRAINT is_signed_valid  CHECK (is_signed IN (0,1)),
    CONSTRAINT mux_type_valid   CHECK (mux_type IN (0,1,2)),
    CONSTRAINT mux_value_valid   CHECK (
        (mux_type IN (0, 1) AND mux_value IS NULL) OR
        (mux_type = 2 AND mux_value IS NOT NULL AND mux_value >= 0)
    ),

    UNIQUE (message_id, name)
);

CREATE TABLE signal_values (
    id          INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    signal_id   INTEGER NOT NULL REFERENCES signals(id) ON DELETE CASCADE,
    raw_value   INTEGER NOT NULL,
    description TEXT    NOT NULL DEFAULT '',

    UNIQUE (signal_id, raw_value)
);

CREATE TABLE signal_receivers (
    signal_id INTEGER NOT NULL REFERENCES signals(id) ON DELETE CASCADE,
    device_id INTEGER NOT NULL REFERENCES devices(id) ON DELETE CASCADE,

    PRIMARY KEY (signal_id, device_id)
);

-- Enforce CAN FD messages are only on FD buses.
CREATE TRIGGER trg_stop_fd_on_standard_bus
    BEFORE INSERT ON messages
BEGIN
    SELECT
        CASE
            WHEN NEW.is_fd = 1 AND (SELECT is_fd FROM buses WHERE id = NEW.bus_id) = 0
                THEN RAISE(ABORT, 'Cannot add a CAN FD message to a standard bus.')
            END;
END;
CREATE TRIGGER trg_stop_fd_on_standard_bus_update
    BEFORE UPDATE OF is_fd, bus_id ON messages
BEGIN
    SELECT
        CASE
            WHEN NEW.is_fd = 1 AND (SELECT is_fd FROM buses WHERE id = NEW.bus_id) = 0
                THEN RAISE(ABORT, 'Cannot add a CAN FD message to a standard bus.')
            END;
END;

-- Enforce that a signal fits in message bounds
CREATE TRIGGER trg_check_signal_bounds
    BEFORE INSERT ON SIGNALS
BEGIN
    SELECT CASE
        WHEN (NEW.start_bit + NEW.bit_length) > ((SELECT dlc FROM messages WHERE id = NEW.message_id) * 8)
        THEN RAISE(ABORT, 'Signal exceeds message bounds.')
    END;
END;

-- Enforce that two signals can't collide
CREATE TRIGGER trg_check_signal_collision
    BEFORE INSERT ON signals
BEGIN
    SELECT CASE
        WHEN EXISTS (
            SELECT 1 FROM signals WHERE message_id = NEW.message_id
                                  -- Bit overlap check
                                  AND NEW.start_bit < (start_bit + bit_length)
                                  AND start_bit < (NEW.start_bit + NEW.bit_length)
                                  -- Multiplex check
                                  AND (
                                      -- Both are normal unmultiplexed signals
                                      (NEW.mux_type = 0 AND NEW.mux_value = 0)
                                      -- Both belong to same mux state
                                      OR (NEW.mux_type = 2 AND mux_type = 2 AND mux_value = mux_value)
                                      -- One is static and other is multiplexed
                                      OR (NEW.mux_type = 0 AND mux_type != 0)
                                      OR (NEW.mux_type != 1 AND mux_type = 1)
                                 )
        )
        THEN RAISE(ABORT, 'Signal collides with another signal.')
    END;
END;

-- Enforce that a transmitter is connected to a bus
CREATE TRIGGER trg_validate_transmitter_bus
    BEFORE INSERT ON messages
    WHEN NEW.transmitter_id IS NOT NULL
BEGIN
    SELECT CASE
               WHEN NOT EXISTS (
                   SELECT 1 FROM bus_connections
                   WHERE device_id = NEW.transmitter_id AND bus_id = NEW.bus_id
               )
                   THEN RAISE(ABORT, 'Transmitter device is not connected to this bus.')
               END;
END;

CREATE TRIGGER trg_validate_signal_bounds_insert
    BEFORE INSERT ON signals
BEGIN
    SELECT CASE
               -- Little-Endian:
               WHEN NEW.byte_order = 0 AND (NEW.start_bit + NEW.bit_length) > (
                   (SELECT dlc FROM messages WHERE id = NEW.message_id) * 8
                   )
                   THEN RAISE(ABORT, 'Little-Endian signal exceeds message DLC.')
               -- Big-Endian:
               WHEN NEW.byte_order = 1 AND (
                                               (NEW.start_bit / 8) +
                                               CAST(MAX(0, (NEW.bit_length - (NEW.start_bit % 8) - 1 + 7) / 8) AS INT)
                                               ) >= (SELECT dlc FROM messages WHERE id = NEW.message_id)
                   THEN RAISE(ABORT, 'Big-Endian signal exceeds message DLC.')
               END;
END;
CREATE TRIGGER trg_validate_signal_bounds_update
    BEFORE UPDATE OF start_bit, bit_length, byte_order, message_id ON signals
BEGIN
    SELECT CASE
               -- Little-Endian:
               WHEN NEW.byte_order = 0 AND (NEW.start_bit + NEW.bit_length) > (
                   (SELECT dlc FROM messages WHERE id = NEW.message_id) * 8
                   )
                   THEN RAISE(ABORT, 'Little-Endian signal exceeds message DLC.')
               -- Big-Endian:
               WHEN NEW.byte_order = 1 AND (
                                               (NEW.start_bit / 8) +
                                               CAST(MAX(0, (NEW.bit_length - (NEW.start_bit % 8) - 1 + 7) / 8) AS INT)
                                               ) >= (SELECT dlc FROM messages WHERE id = NEW.message_id)
                   THEN RAISE(ABORT, 'Big-Endian signal exceeds message DLC.')
               END;
END;

CREATE INDEX idx_signals_message_id   ON signals(message_id);
CREATE INDEX idx_messages_bus_id      ON messages(bus_id);
CREATE INDEX idx_messages_transmitter ON messages(transmitter_id);
CREATE INDEX idx_bus_conn_bus         ON bus_connections(bus_id);
CREATE INDEX idx_sig_val_signal       ON signal_values(signal_id);
CREATE INDEX idx_sig_rec_device       ON signal_receivers(device_id);