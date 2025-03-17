
import java.lang.foreign.*;
import java.lang.invoke.*;
import java.time.Duration;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

public class AudioAPI {

    private static final SymbolLookup LOOKUP;
    private static final Linker LINKER;
    private static final MethodHandle CREATE_AUDIO_MANAGER;
    private static final MethodHandle DESTROY_AUDIO_MANAGER;
    private static final MethodHandle INITIALIZE;
    private static final MethodHandle GET_DURATION;
    private static final MethodHandle GET_CURRENT_POSITION;
    private static final MethodHandle GET_TITLE;
    private static final MethodHandle GET_ARTIST;
    private static final MethodHandle GET_ALBUM;
    private static final MethodHandle PLAY;
    private static final MethodHandle PAUSE;
    private static final MethodHandle NEXT;
    private static final MethodHandle PREVIOUS;
    private static final MethodHandle SEEK;
    private static final MethodHandle SET_VOLUME;
    private static final MethodHandle GET_VOLUME;
    private static final MethodHandle SET_PLAYBACK_CALLBACK;
    private static final MethodHandle SET_TRACK_CALLBACK;
    private static final MethodHandle GET_THUMBNAIL_BYTES;

    private static final MemoryLayout EXPECTED_RESULT_LAYOUT = MemoryLayout.structLayout(
            ValueLayout.JAVA_BOOLEAN.withName("has_value"),
            MemoryLayout.paddingLayout(7),
            ValueLayout.ADDRESS.withName("value_or_error")
    );

    static {
        System.loadLibrary("Music");

        LOOKUP = SymbolLookup.loaderLookup();
        LINKER = Linker.nativeLinker();

        CREATE_AUDIO_MANAGER = linkerFunction("createAudioManager",
                FunctionDescriptor.of(ValueLayout.ADDRESS));

        DESTROY_AUDIO_MANAGER = linkerFunction("destroyAudioManager",
                FunctionDescriptor.ofVoid(ValueLayout.ADDRESS));

        INITIALIZE = linkerFunction("initialize",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        GET_DURATION = linkerFunction("getDuration",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        GET_CURRENT_POSITION = linkerFunction("getCurrentPosition",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        GET_TITLE = linkerFunction("getTitle",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        GET_ARTIST = linkerFunction("getArtist",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        GET_ALBUM = linkerFunction("getAlbum",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        PLAY = linkerFunction("play",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        PAUSE = linkerFunction("pause",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        NEXT = linkerFunction("next",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        PREVIOUS = linkerFunction("previous",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        SEEK = linkerFunction("seek",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS, ValueLayout.JAVA_LONG));

        SET_VOLUME = linkerFunction("setVolume",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS, ValueLayout.JAVA_DOUBLE));

        GET_VOLUME = linkerFunction("getVolume",
                FunctionDescriptor.of(EXPECTED_RESULT_LAYOUT, ValueLayout.ADDRESS));

        SET_PLAYBACK_CALLBACK = linkerFunction("setPlaybackCallback",
                FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS));

        SET_TRACK_CALLBACK = linkerFunction("setTrackCallback",
                FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS));
        GET_THUMBNAIL_BYTES = linkerFunction("getThumbnailBytes",
                FunctionDescriptor.of(
                        EXPECTED_RESULT_LAYOUT,
                        ValueLayout.ADDRESS,
                        ValueLayout.ADDRESS,
                        ValueLayout.ADDRESS
                ));

    }

    private static MethodHandle linkerFunction(String name, FunctionDescriptor descriptor) {
        return LINKER.downcallHandle(
                LOOKUP.find(name).orElseThrow(() -> new RuntimeException("Symbol not found: " + name)),
                descriptor);
    }

    public static class AudioManager implements AutoCloseable {

        private final MemorySegment nativeHandle;
        private boolean closed = false;

        private final PlaybackCallbackStub playbackCallbackStub;
        private final TrackChangedCallbackStub trackChangedCallbackStub;

        public AudioManager() {
            try {
                this.nativeHandle = (MemorySegment) CREATE_AUDIO_MANAGER.invokeExact();
                this.playbackCallbackStub = new PlaybackCallbackStub();
                this.trackChangedCallbackStub = new TrackChangedCallbackStub();
            } catch (Throwable e) {
                throw new RuntimeException("Failed to create AudioManager", e);
            }
        }

        public void initialize() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var result = (MemorySegment) INITIALIZE.invokeExact(SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT)), nativeHandle);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to initialize AudioManager", e);
            }
        }

        public Duration getDuration() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_DURATION.invokeExact(allocator, nativeHandle);
                final var seconds = getValueAsLong(result);
                return Duration.ofSeconds(seconds);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get duration", e);
            }
        }

        public Duration getCurrentPosition() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
           final var result = (MemorySegment) GET_CURRENT_POSITION.invokeExact(allocator, nativeHandle);
                final var seconds = getValueAsLong(result);
                return Duration.ofSeconds(seconds);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get current position", e);
            }
        }

        public String getTitle() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_TITLE.invokeExact(allocator, nativeHandle);
                return getValueAsString(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get title", e);
            }
        }

        public String getArtist() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_ARTIST.invokeExact(allocator, nativeHandle);
                return getValueAsString(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get artist", e);
            }
        }

        public byte[] getThumbnailBytes() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var outBufferPtr = arena.allocate(ValueLayout.ADDRESS);
                final var outSizePtr = arena.allocate(ValueLayout.ADDRESS);
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_THUMBNAIL_BYTES.invokeExact(allocator,
                        nativeHandle,
                        outBufferPtr,
                        outSizePtr
                );
                checkResult(result);
                final var bufferPtr = MemorySegment.ofAddress(outBufferPtr.get(ValueLayout.ADDRESS, 0).address());
                final var size = outSizePtr.get(ValueLayout.JAVA_LONG, 0);
                final var bytes = new byte[(int) size];
                final var dataSegment = bufferPtr.reinterpret(size);
                for (var i = 0; i < size; i++) {
                    bytes[i] = dataSegment.get(ValueLayout.JAVA_BYTE, i);
                }
                return bytes;
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get thumbnail bytes", e);
            }
        }

        public String getAlbum() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_ALBUM.invokeExact(allocator, nativeHandle);
                return getValueAsString(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get album", e);
            }
        }

        public void play() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) PLAY.invokeExact(allocator, nativeHandle);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to play", e);
            }
        }

        public void pause() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) PAUSE.invokeExact(allocator, nativeHandle);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to pause", e);
            }
        }

        public void next() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) NEXT.invokeExact(allocator, nativeHandle);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to skip to next track", e);
            }
        }

        public void previous() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) PREVIOUS.invokeExact(allocator, nativeHandle);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to go to previous track", e);
            }
        }

        public void seek(Duration position) throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) SEEK.invokeExact(allocator, nativeHandle,position.getSeconds());
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to seek", e);
            }
        }

        public void setVolume(double volume) throws AudioException {
            checkClosed();
            if (volume < 0.0 || volume > 1.0) {
                throw new IllegalArgumentException("Volume must be between 0.0 and 1.0");
            }
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) SET_VOLUME.invokeExact(allocator, nativeHandle, volume);
                checkResult(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to set volume", e);
            }
        }

        public double getVolume() throws AudioException {
            checkClosed();
            try (final var arena = Arena.ofConfined()) {
                final var allocator = SegmentAllocator.slicingAllocator(arena.allocate(EXPECTED_RESULT_LAYOUT));
                final var result = (MemorySegment) GET_VOLUME.invokeExact(allocator, nativeHandle);
                return getValueAsDouble(result);
            } catch (AudioException e) {
                throw e;
            } catch (Throwable e) {
                throw new RuntimeException("Failed to get volume", e);
            }
        }

        public void setPlaybackChangedCallback(Consumer<String> callback) {
            checkClosed();
            try {
                playbackCallbackStub.setCallback(callback);
                SET_PLAYBACK_CALLBACK.invokeExact(nativeHandle, playbackCallbackStub.segment);
            } catch (Throwable e) {
                throw new RuntimeException("Failed to set playback callback", e);
            }
        }

        public void setTrackChangedCallback(BiConsumer<String, String> callback) {
            checkClosed();
            try {
                trackChangedCallbackStub.setCallback(callback);
                SET_TRACK_CALLBACK.invokeExact(nativeHandle, trackChangedCallbackStub.segment);
            } catch (Throwable e) {
                throw new RuntimeException("Failed to set track changed callback", e);
            }
        }

        @Override
        public void close() {
            if (!closed) {
                try {
                    try (trackChangedCallbackStub; playbackCallbackStub) {
                        DESTROY_AUDIO_MANAGER.invokeExact(nativeHandle);
                    }
                    closed = true;
                } catch (Throwable e) {
                    throw new RuntimeException("Failed to close AudioManager", e);
                }
            }
        }

        private void checkClosed() {
            if (closed) {
                throw new IllegalStateException("AudioManager is closed");
            }
        }

        private void checkResult(MemorySegment result) throws AudioException {
            final var hasValue = result.get(ValueLayout.JAVA_BOOLEAN, 0);
            if (!hasValue) {
                MemorySegment errorMsg = result.get(ValueLayout.ADDRESS, 8);
                throw new AudioException(errorToString(errorMsg));
            }
        }

        private String getValueAsString(MemorySegment result) throws AudioException {
            final var hasValue = result.get(ValueLayout.JAVA_BOOLEAN, 0);
            MemorySegment valuePtr = result.get(ValueLayout.ADDRESS, 8);

            if (!hasValue) {
                throw new AudioException(errorToString(valuePtr));
            }
            try {
                final var stringSegment = MemorySegment.ofAddress(valuePtr.address()).reinterpret(Long.MAX_VALUE);
                return stringSegment.getString(0);
            } catch (Exception e) {
                throw new RuntimeException("Failed to read string value", e);
            }
        }

        private long getValueAsLong(MemorySegment result) throws AudioException {
            final var hasValue = result.get(ValueLayout.JAVA_BOOLEAN, 0);
            final var valuePtr = result.get(ValueLayout.ADDRESS, 8);

            if (!hasValue) {
                throw new AudioException(errorToString(valuePtr));
            }
            try {
                final var valueSegment = MemorySegment.ofAddress(valuePtr.address()).reinterpret(8);
                return valueSegment.get(ValueLayout.JAVA_LONG, 0);
            } catch (Exception e) {
                throw new RuntimeException("Failed to read long value", e);
            }
        }

        private double getValueAsDouble(MemorySegment result) throws AudioException {
            final var hasValue = result.get(ValueLayout.JAVA_BOOLEAN, 0);
            final var valuePtr = result.get(ValueLayout.ADDRESS, 8);

            if (!hasValue) {
                throw new AudioException(errorToString(valuePtr));
            }

            try {
                final var valueSegment = MemorySegment.ofAddress(valuePtr.address()).reinterpret(8);
                return valueSegment.get(ValueLayout.JAVA_DOUBLE, 0);
            } catch (Exception e) {
                throw new RuntimeException("Failed to read double value", e);
            }
        }

        private String errorToString(MemorySegment error) {
            try {
                final var stringSegment = MemorySegment.ofAddress(error.address()).reinterpret(Long.MAX_VALUE);
                return stringSegment.getString(0);
            } catch (Exception e) {
                return "Unknown error (could not read error message)";
            }
        }
    }

    public static class AudioException extends Exception {

        public AudioException(String message) {
            super(message);
        }
    }

    private static class PlaybackCallbackStub implements AutoCloseable {

        private static final Arena CALLBACK_ARENA = Arena.global();
        private Consumer<String> callback;
        final MemorySegment segment;

        PlaybackCallbackStub() {
            segment = CALLBACK_ARENA.allocate(ValueLayout.ADDRESS);
            MethodHandle upCall;
            try {
                upCall = MethodHandles.lookup().findVirtual(
                        PlaybackCallbackStub.class,
                        "invoke",
                        MethodType.methodType(void.class, MemorySegment.class));
            } catch (IllegalAccessException | NoSuchMethodException e) {
                throw new RuntimeException(e);
            }
            final var boundUpCall = upCall.bindTo(this);
            final var upcallStub = LINKER.upcallStub(
                    boundUpCall,
                    FunctionDescriptor.ofVoid(ValueLayout.ADDRESS),
                    CALLBACK_ARENA);
            segment.set(ValueLayout.ADDRESS, 0, upcallStub);
        }

        public void invoke(MemorySegment messagePtr) {
            if (callback != null) {
                String message = messagePtr.getString(0);
                callback.accept(message);
            }
        }

        public void setCallback(Consumer<String> callback) {
            this.callback = callback;
        }

        @Override
        public void close() {
        }
    }

    private static class TrackChangedCallbackStub implements AutoCloseable {

        private static final Arena CALLBACK_ARENA = Arena.global();
        private BiConsumer<String, String> callback;
        final MemorySegment segment;

        TrackChangedCallbackStub() {
            segment = CALLBACK_ARENA.allocate(ValueLayout.ADDRESS);
            MethodHandle upCall;
            try {
                upCall = MethodHandles.lookup().findVirtual(
                        TrackChangedCallbackStub.class,
                        "invoke",
                        MethodType.methodType(void.class, MemorySegment.class, MemorySegment.class));
            } catch (IllegalAccessException | NoSuchMethodException e) {
                throw new RuntimeException(e);
            }
            final var boundUpCall = upCall.bindTo(this);
            final var upcallStub = LINKER.upcallStub(
                    boundUpCall,
                    FunctionDescriptor.ofVoid(ValueLayout.ADDRESS, ValueLayout.ADDRESS),
                    CALLBACK_ARENA);
            segment.set(ValueLayout.ADDRESS, 0, upcallStub);
        }

        public void invoke(MemorySegment titlePtr, MemorySegment artistPtr) {
            if (callback != null) {
                String title = titlePtr.getString(0);
                String artist = artistPtr.getString(0);
                callback.accept(title, artist);
            }
        }

        public void setCallback(BiConsumer<String, String> callback) {
            this.callback = callback;
        }

        @Override
        public void close() {
        }
    }
}
